#include "kb.hpp"
#include "downloader.hpp"

#include <curl/curl.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

namespace kb {
namespace fs = std::filesystem;
namespace {

// Kiwix index URL listing all Wikipedia ZIM files. We parse the simple
// directory listing for the newest wikipedia_en_all_nopic_YYYY-MM.zim
// and download it.
constexpr const char * kKiwixIndex =
    "https://download.kiwix.org/zim/wikipedia/";
constexpr const char * kZimPattern = R"(wikipedia_en_all_nopic_\d{4}-\d{2}\.zim)";

constexpr const char * kZimFilename  = "wikipedia.zim";
constexpr const char * kStateFile    = "wikipedia.download.json";

struct Globals {
    fs::path           data_dir;
    fs::path           zim_path;
    fs::path           state_path;
    WikipediaArchive   archive;
    Downloader         downloader;
    std::thread        watcher;
    std::atomic<bool>  watcher_stop{false};
    std::string        latest_url;
    std::mutex         mtx;
};

Globals & G() { static Globals g; return g; }

std::size_t curl_collect(char * ptr, std::size_t sz, std::size_t nm, void * ud) {
    auto * s = static_cast<std::string *>(ud);
    s->append(ptr, sz * nm);
    return sz * nm;
}

bool fetch_index_html(std::string & html, std::string & err) {
    CURL * c = curl_easy_init();
    if (!c) { err = "curl init"; return false; }
    char errbuf[CURL_ERROR_SIZE] = {0};
    html.clear();
    curl_easy_setopt(c, CURLOPT_URL,            kKiwixIndex);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION,  curl_collect);
    curl_easy_setopt(c, CURLOPT_WRITEDATA,      &html);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_FAILONERROR,    1L);
    curl_easy_setopt(c, CURLOPT_ERRORBUFFER,    errbuf);
    curl_easy_setopt(c, CURLOPT_TIMEOUT,        20L);
    curl_easy_setopt(c, CURLOPT_USERAGENT,      "tool/kb");
    CURLcode rc = curl_easy_perform(c);
    if (rc != CURLE_OK) {
        err = errbuf[0] ? errbuf : curl_easy_strerror(rc);
        curl_easy_cleanup(c);
        return false;
    }
    curl_easy_cleanup(c);
    return true;
}

// Scan the index HTML, return the URL of the lexicographically latest
// wikipedia_en_all_nopic_YYYY-MM.zim file.
bool resolve_latest_url(std::string & out_url, std::string & err) {
    std::string html;
    if (!fetch_index_html(html, err)) return false;

    std::regex re(kZimPattern);
    auto begin = std::sregex_iterator(html.begin(), html.end(), re);
    auto end   = std::sregex_iterator();
    std::string best;
    for (auto it = begin; it != end; ++it) {
        const std::string m = it->str();
        if (m > best) best = m;
    }
    if (best.empty()) {
        err = "no nopic zim found in kiwix index";
        return false;
    }
    out_url = std::string(kKiwixIndex) + best;
    return true;
}

void try_open_existing_zim_locked() {
    if (fs::exists(G().zim_path) && fs::file_size(G().zim_path) > 0) {
        // Only open if a previous download claimed completion (no resume
        // state file present), else the file may be partial.
        if (!fs::exists(G().state_path)) {
            (void) G().archive.open(G().zim_path);
        }
    }
}

void watcher_thread() {
    // 1. If we already have a complete-looking ZIM, try to open it.
    {
        std::lock_guard<std::mutex> lk(G().mtx);
        try_open_existing_zim_locked();
    }

    // 2. Resolve the latest URL on Kiwix. If we can't reach the network,
    //    just exit (KB stays at whatever state it's in -- ready if a
    //    pre-existing ZIM was opened, offline otherwise).
    std::string url, err;
    if (!resolve_latest_url(url, err)) {
        std::fprintf(stderr, "kb: cannot resolve Wikipedia index (%s); "
                             "knowledge will be offline this run\n", err.c_str());
        return;
    }
    {
        std::lock_guard<std::mutex> lk(G().mtx);
        G().latest_url = url;
    }

    // 3. Decide whether to download. We download when:
    //    - no zim file present, OR
    //    - resume state exists (partial download), OR
    //    - existing zim was produced from a different URL than the latest.
    bool need_download = false;
    {
        std::lock_guard<std::mutex> lk(G().mtx);
        if (!fs::exists(G().zim_path)) need_download = true;
        else if (fs::exists(G().state_path)) need_download = true;
        // We don't yet track which URL produced an existing zim, so we
        // conservatively skip update-on-version-change for now. The user
        // can force a refresh by deleting wikipedia.zim.
    }

    if (need_download) {
        G().downloader.start(url, G().zim_path, G().state_path);
    }

    // 4. Poll until the download is done (or we're told to stop). When
    //    it's done, open the resulting zim and remove the state file.
    while (!G().watcher_stop.load()) {
        if (need_download) {
            auto st = G().downloader.status();
            if (st.done) {
                std::lock_guard<std::mutex> lk(G().mtx);
                if (G().archive.open(G().zim_path)) {
                    std::error_code ec;
                    fs::remove(G().state_path, ec);
                    std::fprintf(stderr, "kb: Wikipedia ZIM mounted (%llu bytes)\n",
                                 static_cast<unsigned long long>(st.total_bytes));
                }
                return;
            }
            if (!st.error.empty()) {
                std::fprintf(stderr, "kb: download error: %s\n", st.error.c_str());
                return;
            }
        } else {
            return;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

}

void init(const std::string & data_dir) {
    static std::once_flag once;
    std::call_once(once, []{ curl_global_init(CURL_GLOBAL_DEFAULT); });

    std::lock_guard<std::mutex> lk(G().mtx);
    G().data_dir   = data_dir;
    G().zim_path   = G().data_dir / kZimFilename;
    G().state_path = G().data_dir / kStateFile;
    fs::create_directories(G().data_dir);

    G().watcher_stop.store(false);
    G().watcher = std::thread(&watcher_thread);
}

void shutdown() {
    G().watcher_stop.store(true);
    G().downloader.stop();
    if (G().watcher.joinable()) G().watcher.join();
    G().archive.close();
}

LookupResult suggest(std::string_view query, int max_results) {
    LookupResult r;
    r.available = G().archive.is_open();
    if (!r.available) return r;
    r.hits = G().archive.suggest(query, max_results);
    return r;
}

LookupResult search(std::string_view query, int max_results) {
    LookupResult r;
    r.available = G().archive.is_open();
    if (!r.available) return r;
    r.hits = G().archive.search(query, max_results);
    return r;
}

std::string render_for_prompt(std::string_view query, int max_results) {
    auto r = suggest(query, max_results);
    if (!r.available) return "(knowledge offline)";
    if (r.hits.empty()) return "(no matches)";
    std::string out;
    for (const auto & h : r.hits) {
        out.append("- ");
        out.append(h.title);
        if (!h.snippet.empty()) {
            out.append(": ");
            out.append(h.snippet);
        }
        out.push_back('\n');
    }
    return out;
}

std::string status_string() {
    if (G().archive.is_open()) {
        std::uint64_t sz = 0;
        std::error_code ec;
        sz = fs::file_size(G().zim_path, ec);
        char buf[64];
        std::snprintf(buf, sizeof(buf), "ready (%.1f GiB)",
                      static_cast<double>(sz) / (1024.0 * 1024.0 * 1024.0));
        return buf;
    }
    auto st = G().downloader.status();
    if (st.active && st.total_bytes > 0) {
        char buf[160];
        const double pct = 100.0 * static_cast<double>(st.downloaded_bytes) /
                                  static_cast<double>(st.total_bytes);
        std::snprintf(buf, sizeof(buf),
                      "downloading %.1f%% (%.2f/%.2f GiB, chunks %zu/%zu)",
                      pct,
                      static_cast<double>(st.downloaded_bytes) / (1024.0*1024.0*1024.0),
                      static_cast<double>(st.total_bytes)      / (1024.0*1024.0*1024.0),
                      st.chunks_complete, st.chunks_total);
        return buf;
    }
    if (!st.error.empty()) return "error: " + st.error;
    return "offline (no zim mounted)";
}

}
