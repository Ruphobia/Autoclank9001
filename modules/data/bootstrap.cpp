// SPDX-License-Identifier: GPL-3.0-or-later
#include "bootstrap.hpp"
#include "data.hpp"

#include <curl/curl.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace data {
namespace fs = std::filesystem;
using json    = nlohmann::json;

namespace {

std::once_flag g_curl_init_once;

void curl_global_init_once() {
    std::call_once(g_curl_init_once, [] {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        // No global_cleanup: we run for the process lifetime and
        // atexit is fine.
    });
}

// Bytes -> human size for progress lines.
std::string hb(std::uintmax_t n) {
    char b[64];
    if (n >= (1ULL << 30))
        std::snprintf(b, sizeof(b), "%.2f GB",
                      static_cast<double>(n) / (1ULL << 30));
    else if (n >= (1ULL << 20))
        std::snprintf(b, sizeof(b), "%.1f MB",
                      static_cast<double>(n) / (1ULL << 20));
    else
        std::snprintf(b, sizeof(b), "%ju B",
                      static_cast<std::uintmax_t>(n));
    return b;
}

std::size_t write_to_file(void * ptr, std::size_t size, std::size_t nmemb,
                          void * userp) {
    auto * f = static_cast<std::FILE *>(userp);
    return std::fwrite(ptr, size, nmemb, f);
}

int progress_cb(void * clientp, curl_off_t dltotal, curl_off_t dlnow,
                curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) {
    if (dltotal == 0) return 0;
    auto * last = static_cast<curl_off_t *>(clientp);
    // Rate-limit progress prints: every ~2% of the file.
    curl_off_t step = dltotal / 50;
    if (step > 0 && dlnow - *last < step) return 0;
    *last = dlnow;
    int    pct = static_cast<int>((dlnow * 100) / dltotal);
    std::fprintf(stderr,
        "\r  downloading: %3d%% (%s / %s)      ",
        pct,
        hb(static_cast<std::uintmax_t>(dlnow)).c_str(),
        hb(static_cast<std::uintmax_t>(dltotal)).c_str());
    std::fflush(stderr);
    return 0;
}

// Download a URL to `out_path` with libcurl. Returns true on success.
bool download_to_file(const std::string & url,
                      const fs::path    & out_path) {
    curl_global_init_once();
    CURL * curl = curl_easy_init();
    if (!curl) return false;

    std::FILE * out = std::fopen(out_path.c_str(), "wb");
    if (!out) {
        std::fprintf(stderr, "bootstrap: cannot open %s for writing\n",
                     out_path.c_str());
        curl_easy_cleanup(curl);
        return false;
    }

    curl_off_t last_dlnow = 0;

    curl_easy_setopt(curl, CURLOPT_URL,             url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION,  1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS,       10L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL,        1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME,  30L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1024L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
                     "ac9-bootstrap/1 (autoclank9001)");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,   write_to_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,       out);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS,      0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_cb);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA,    &last_dlnow);

    std::fprintf(stderr, "  GET %s\n", url.c_str());
    CURLcode rc = curl_easy_perform(curl);
    std::fclose(out);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    std::fprintf(stderr, "\n");
    if (rc != CURLE_OK) {
        std::fprintf(stderr,
            "bootstrap: curl failed for %s: %s\n",
            url.c_str(), curl_easy_strerror(rc));
        std::error_code ec; fs::remove(out_path, ec);
        return false;
    }
    if (http_code >= 400) {
        std::fprintf(stderr,
            "bootstrap: HTTP %ld for %s\n", http_code, url.c_str());
        std::error_code ec; fs::remove(out_path, ec);
        return false;
    }
    return true;
}

json load_json_file(const fs::path & p) {
    std::ifstream f(p);
    if (!f) return json::object();
    try { return json::parse(f); }
    catch (...) { return json::object(); }
}

bool role_in_manifest(const fs::path    & data_dir,
                      const std::string & role) {
    auto m = load_json_file(data_dir / "manifest.json");
    return m.contains(role) && m[role].contains("chunks");
}

}  // namespace

bool bootstrap_role(const fs::path      & data_dir,
                    const std::string   & role,
                    bool                  force) {
    { std::error_code _ec; fs::create_directories(data_dir, _ec); }
    const auto sources = load_json_file(data_dir / "sources.json");
    if (!sources.contains(role)) {
        std::fprintf(stderr,
            "bootstrap: role \"%s\" not in %s/sources.json\n",
            role.c_str(), data_dir.c_str());
        return false;
    }
    if (!force && role_in_manifest(data_dir, role)) {
        std::fprintf(stderr,
            "bootstrap: role \"%s\" already in manifest, skipping "
            "(use --force to re-fetch)\n", role.c_str());
        return true;
    }
    const auto & entry = sources[role];
    const std::string human = entry.value("human_name", role + ".bin");
    std::vector<std::string> urls;
    if (entry.contains("sources") && entry["sources"].is_array()) {
        for (const auto & u : entry["sources"]) {
            if (u.is_string()) urls.push_back(u.get<std::string>());
        }
    }
    if (urls.empty()) {
        std::fprintf(stderr,
            "bootstrap: role \"%s\" has no sources configured; "
            "skipping\n", role.c_str());
        return false;
    }

    { std::error_code _ec; fs::create_directories(data_dir / "staging", _ec); }
    const fs::path staging = data_dir / "staging" / human;

    bool ok = false;
    for (const auto & url : urls) {
        std::fprintf(stderr,
            "bootstrap: role \"%s\" -> %s\n", role.c_str(), human.c_str());
        if (download_to_file(url, staging)) { ok = true; break; }
    }
    if (!ok) {
        std::fprintf(stderr,
            "bootstrap: all sources failed for role \"%s\"\n",
            role.c_str());
        return false;
    }

    try {
        auto res = chunk_asset(staging, data_dir);
        set_role_in_manifest(data_dir, role, human, res);
        std::fprintf(stderr,
            "bootstrap: role \"%s\" ready (%zu chunks, sha %s)\n",
            role.c_str(), res.chunk_shas.size(),
            res.full_sha256.c_str());
    } catch (const std::exception & ex) {
        std::fprintf(stderr,
            "bootstrap: chunking failed for \"%s\": %s\n",
            role.c_str(), ex.what());
        std::error_code ec; fs::remove(staging, ec);
        return false;
    }
    std::error_code ec;
    fs::remove(staging, ec);
    return true;
}

int bootstrap(const fs::path & data_dir) {
    const auto sources = load_json_file(data_dir / "sources.json");
    if (sources.empty()) {
        std::fprintf(stderr,
            "bootstrap: no %s/sources.json, nothing to do\n",
            data_dir.c_str());
        return 0;
    }
    std::vector<std::string> todo;
    for (auto it = sources.begin(); it != sources.end(); ++it) {
        const std::string role = it.key();
        if (role.empty() || role[0] == '_') continue;   // _note / _todo
        if (role_in_manifest(data_dir, role)) continue;
        // Skip roles with no URL yet; they're placeholders.
        if (!it.value().contains("sources") ||
            !it.value()["sources"].is_array() ||
            it.value()["sources"].empty()) {
            std::fprintf(stderr,
                "bootstrap: role \"%s\" has no source URL yet, skipping\n",
                role.c_str());
            continue;
        }
        todo.push_back(role);
    }
    if (todo.empty()) {
        std::fprintf(stderr,
            "bootstrap: manifest already covers every configured role\n");
        return 0;
    }
    std::fprintf(stderr,
        "bootstrap: %zu roles to fetch: ", todo.size());
    for (std::size_t i = 0; i < todo.size(); ++i) {
        std::fprintf(stderr, "%s%s",
                     todo[i].c_str(),
                     i + 1 < todo.size() ? ", " : "\n");
    }

    int done = 0;
    for (const auto & role : todo) {
        if (bootstrap_role(data_dir, role)) ++done;
    }
    std::fprintf(stderr,
        "bootstrap: %d/%zu roles ready\n", done,
        todo.size());
    return done;
}

int cli_fetch(int argc, char ** argv) {
    // ac9 fetch [role] [--force] [--data DIR]
    std::string role;
    bool force = false;
    fs::path data_dir = "data";
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--force") force = true;
        else if (a == "--data" && i + 1 < argc) { data_dir = argv[++i]; }
        else if (!a.empty() && a[0] != '-')     { role = a; }
        else {
            std::fprintf(stderr,
                "unknown option: %s\n"
                "usage: %s fetch [role] [--force] [--data DIR]\n",
                a.c_str(), argv[0]);
            return 2;
        }
    }
    try {
        if (role.empty()) {
            bootstrap(data_dir);
            return 0;
        }
        return bootstrap_role(data_dir, role, force) ? 0 : 1;
    } catch (const std::exception & ex) {
        std::fprintf(stderr, "bootstrap: %s\n", ex.what());
        return 1;
    }
}

}  // namespace data
