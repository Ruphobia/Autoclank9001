// SPDX-License-Identifier: GPL-3.0-or-later
#include "office_docs.hpp"

#include "../shell/coder.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

namespace office_docs {

namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace {

// One conversion at a time. soffice --headless is not multi-instance
// safe when several callers share the same UserInstallation profile.
// A global mutex keeps the queue orderly at the cost of some parallelism
// (fine: doc reads are rare relative to LLM calls).
std::mutex g_conv_mtx;

// (path, mtime_ns) -> extracted plain text. Populated by read() so a
// hot loop of doc_read tool calls does not re-shell soffice on every
// call.
struct CacheKey {
    std::string     path;
    std::int64_t    mtime_ns = 0;
    bool operator==(const CacheKey & o) const noexcept {
        return mtime_ns == o.mtime_ns && path == o.path;
    }
};
struct CacheKeyHash {
    std::size_t operator()(const CacheKey & k) const noexcept {
        return std::hash<std::string>{}(k.path) ^
               static_cast<std::size_t>(k.mtime_ns);
    }
};
std::mutex                                                       g_cache_mtx;
std::unordered_map<CacheKey, std::string, CacheKeyHash>          g_text_cache;

std::string env_or(const char * key, const std::string & fallback) {
    const char * v = std::getenv(key);
    return (v && *v) ? std::string(v) : fallback;
}

// Loud stderr banner so silent failures are visible.
void warn(const std::string & msg) {
    std::fprintf(stderr, "!!!! OFFICE DOCS !!!! %s\n", msg.c_str());
}

// Load the last-modified time of `path` as nanoseconds since epoch,
// or 0 if the file is unreadable.
std::int64_t mtime_ns_of(const std::string & path) {
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) return 0;
    const auto ft = fs::last_write_time(path, ec);
    if (ec) return 0;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        ft.time_since_epoch()).count();
}

// Lowercase the extension (no leading dot). Empty string when the
// path has no extension.
std::string ext_of(const std::string & path) {
    const auto dot = path.find_last_of('.');
    if (dot == std::string::npos) return {};
    if (dot >= path.size() - 1)   return {};
    std::string e = path.substr(dot + 1);
    for (char & c : e) c = static_cast<char>(std::tolower(
        static_cast<unsigned char>(c)));
    return e;
}

// Filename without directory prefix or extension.
std::string stem_of(const std::string & path) {
    fs::path p(path);
    return p.stem().string();
}

// Fork soffice with the given argv. Returns 0 on success, non-zero on
// process failure, -1 when we failed to even start it. `log` receives
// combined stdout+stderr for diagnostic dumps.
int run_soffice(const std::vector<std::string> & argv, std::string & log) {
    int pipefd[2];
    if (pipe(pipefd) != 0) {
        log = std::string("pipe() failed: ") + std::strerror(errno);
        return -1;
    }
    const pid_t pid = fork();
    if (pid < 0) {
        int e = errno;
        close(pipefd[0]);
        close(pipefd[1]);
        log = std::string("fork() failed: ") + std::strerror(e);
        return -1;
    }
    if (pid == 0) {
        // Child: rewire both stdout+stderr to the pipe. execv with an
        // absolute soffice path so we do not depend on PATH being sane
        // in the ac9 server's environment.
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        std::vector<char *> raw;
        raw.reserve(argv.size() + 1);
        for (const auto & a : argv) raw.push_back(const_cast<char *>(a.c_str()));
        raw.push_back(nullptr);
        execv(argv[0].c_str(), raw.data());
        std::fprintf(stderr, "execv %s failed: %s\n",
                     argv[0].c_str(), std::strerror(errno));
        _exit(127);
    }
    close(pipefd[1]);
    std::array<char, 4096> buf;
    while (true) {
        // Fully qualified: office_docs::read(std::string) is in scope
        // here and would otherwise shadow the POSIX syscall.
        ssize_t n = ::read(pipefd[0], buf.data(), buf.size());
        if (n > 0) log.append(buf.data(), static_cast<std::size_t>(n));
        else if (n == 0) break;
        else if (errno == EINTR) continue;
        else break;
    }
    close(pipefd[0]);
    int wstatus = 0;
    while (waitpid(pid, &wstatus, 0) < 0 && errno == EINTR) {}
    if (WIFEXITED(wstatus))   return WEXITSTATUS(wstatus);
    if (WIFSIGNALED(wstatus)) return 128 + WTERMSIG(wstatus);
    return -1;
}

// Ensure the cache directory exists.
void ensure_cache_dir(const std::string & dir) {
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) warn("mkdir " + dir + " failed: " + ec.message());
}

// Random suffix so parallel-ish requests do not stomp each other in the
// cache directory. soffice appends the source stem to --outdir, so we
// stage the input file under a unique sub-directory to keep outputs
// isolated even when two threads convert siblings with the same stem.
std::string random_suffix() {
    static std::mutex             mtx;
    static std::mt19937_64        eng{
        std::random_device{}() ^
        std::hash<std::thread::id>{}(std::this_thread::get_id())};
    std::lock_guard<std::mutex>   lk(mtx);
    std::uniform_int_distribution<std::uint64_t> dist(1, ~0ull);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%016lx",
                  static_cast<unsigned long>(dist(eng)));
    return std::string(buf);
}

// Read a whole file into memory. Empty on failure.
std::string slurp(const std::string & path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Write text to a path. Creates parent dirs. Returns false on any error.
bool spit(const std::string & path, const std::string & text) {
    std::error_code ec;
    fs::create_directories(fs::path(path).parent_path(), ec);
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(text.data(), static_cast<std::streamsize>(text.size()));
    return static_cast<bool>(f);
}

// Case-insensitive contains. Small helper for whole-word patch matching.
bool ascii_word_char(unsigned char c) {
    return std::isalnum(c) || c == '_';
}

// Apply one patch to `body` in place. Returns the number of matches
// substituted. Whole-word matches require non-word boundaries on both
// sides (or start/end of string).
std::size_t apply_patch(std::string & body, const Patch & p) {
    if (p.find.empty()) return 0;
    std::size_t count = 0;
    std::string out;
    out.reserve(body.size());
    std::size_t i = 0;
    while (i < body.size()) {
        if (body.compare(i, p.find.size(), p.find) == 0) {
            bool ok = true;
            if (p.whole_word) {
                if (i > 0 && ascii_word_char(
                        static_cast<unsigned char>(body[i - 1]))) ok = false;
                const std::size_t after = i + p.find.size();
                if (ok && after < body.size() && ascii_word_char(
                        static_cast<unsigned char>(body[after]))) ok = false;
            }
            if (ok) {
                out.append(p.replace);
                i += p.find.size();
                ++count;
                continue;
            }
        }
        out.push_back(body[i++]);
    }
    body = std::move(out);
    return count;
}

// The soffice convert-to format string for a given extension. Empty
// return means "cannot write this extension".
std::string convert_target(const std::string & ext) {
    if (ext == "odt")  return "odt";
    if (ext == "docx") return "docx";
    if (ext == "ods")  return "ods";
    if (ext == "xlsx") return "xlsx";
    if (ext == "odp")  return "odp";
    if (ext == "pptx") return "pptx";
    if (ext == "odg")  return "odg";
    if (ext == "txt")  return "txt";
    if (ext == "csv")  return "csv";
    if (ext == "html") return "html";
    return {};
}

// Extract heading rows from LibreOffice's HTML export of a text doc.
// Good enough for the LLM to reason about a table of contents without
// bolting on a real ODF parser.
json headings_from_html(const std::string & html) {
    json out = json::array();
    static const std::regex heading_re(
        R"(<h([1-6])[^>]*>(.*?)</h[1-6]>)",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex tag_re(R"(<[^>]+>)");
    auto it  = std::sregex_iterator(html.begin(), html.end(), heading_re);
    auto end = std::sregex_iterator();
    for (; it != end; ++it) {
        int level = std::stoi((*it)[1].str());
        std::string txt = std::regex_replace((*it)[2].str(), tag_re, "");
        // Trim + collapse whitespace.
        std::string trimmed;
        bool last_space = false;
        for (char c : txt) {
            unsigned char u = static_cast<unsigned char>(c);
            if (std::isspace(u)) {
                if (!last_space && !trimmed.empty()) trimmed.push_back(' ');
                last_space = true;
            } else {
                trimmed.push_back(c);
                last_space = false;
            }
        }
        while (!trimmed.empty() && trimmed.back() == ' ') trimmed.pop_back();
        if (trimmed.empty()) continue;
        out.push_back({{"level", level}, {"text", trimmed}});
    }
    return out;
}

// Count slides in a presentation's HTML export. LibreOffice emits an
// <h1> or a marker section per slide; a rough count is fine here.
json slides_from_html(const std::string & html) {
    json out = json::array();
    static const std::regex slide_re(
        R"(<div class=\"?slide\"?[^>]*>(.*?)(?=<div class=\"?slide\"?|</body>))",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex title_re(
        R"(<h[12][^>]*>(.*?)</h[12]>)",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex tag_re(R"(<[^>]+>)");
    auto it  = std::sregex_iterator(html.begin(), html.end(), slide_re);
    auto end = std::sregex_iterator();
    int  idx = 1;
    for (; it != end; ++it, ++idx) {
        std::string block = (*it)[1].str();
        std::smatch tm;
        std::string title;
        if (std::regex_search(block, tm, title_re)) {
            title = std::regex_replace(tm[1].str(), tag_re, "");
        }
        out.push_back({{"index", idx}, {"title", title}});
    }
    // Fallback: if the class="slide" pattern found nothing, split on <h1>.
    if (out.empty()) {
        static const std::regex h1_re(
            R"(<h1[^>]*>(.*?)</h1>)",
            std::regex::icase | std::regex::ECMAScript);
        auto it2  = std::sregex_iterator(html.begin(), html.end(), h1_re);
        auto end2 = std::sregex_iterator();
        idx = 1;
        for (; it2 != end2; ++it2, ++idx) {
            std::string title =
                std::regex_replace((*it2)[1].str(), tag_re, "");
            out.push_back({{"index", idx}, {"title", title}});
        }
    }
    return out;
}

// Extract sheet metadata from LibreOffice's HTML export of a spreadsheet.
// Each sheet becomes a <table>; we count rows via <tr> and columns via
// the widest <tr>'s <td>/<th> count.
json sheets_from_html(const std::string & html) {
    json out = json::array();
    // Sheet name preceded by an <h1>/<h2> in the standard export.
    static const std::regex table_re(
        R"((?:<h[12][^>]*>(.*?)</h[12]>\s*)?<table[^>]*>(.*?)</table>)",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex row_re(
        R"(<tr[^>]*>(.*?)</tr>)",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex cell_re(
        R"(<t[dh][^>]*>)",
        std::regex::icase | std::regex::ECMAScript);
    static const std::regex tag_re(R"(<[^>]+>)");
    auto it  = std::sregex_iterator(html.begin(), html.end(), table_re);
    auto end = std::sregex_iterator();
    int  idx = 1;
    for (; it != end; ++it, ++idx) {
        std::string name =
            std::regex_replace((*it)[1].str(), tag_re, "");
        while (!name.empty() && std::isspace(
                   static_cast<unsigned char>(name.back()))) name.pop_back();
        if (name.empty()) {
            name = "Sheet" + std::to_string(idx);
        }
        std::string tbody = (*it)[2].str();
        std::size_t rows = 0;
        std::size_t cols = 0;
        auto rit  = std::sregex_iterator(tbody.begin(), tbody.end(), row_re);
        auto rend = std::sregex_iterator();
        for (; rit != rend; ++rit) {
            ++rows;
            std::string row = (*rit)[1].str();
            std::size_t c = std::distance(
                std::sregex_iterator(row.begin(), row.end(), cell_re),
                std::sregex_iterator());
            if (c > cols) cols = c;
        }
        out.push_back({
            {"name", name},
            {"rows", rows},
            {"cols", cols},
        });
    }
    return out;
}

// Convert `path` via `soffice --headless --convert-to <target>` and
// return the absolute path of the produced file (in --outdir). Empty
// string on failure.
std::string convert_via_soffice(const std::string & path,
                                const std::string & target_ext,
                                const std::string & work_dir)
{
    ensure_cache_dir(work_dir);
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) {
        warn("cannot convert missing file: " + path);
        return {};
    }
    const std::string soffice = soffice_path();
    if (!fs::exists(soffice, ec)) {
        warn("soffice binary not found at " + soffice +
             " -- set AC9_SOFFICE to override.");
        return {};
    }
    // Per-call profile keeps concurrent conversions from wrestling over
    // ~/.config/libreoffice. We stash it under the same work_dir so it
    // gets cleaned up alongside the output.
    const std::string profile_dir = work_dir + "/profile-" + random_suffix();
    ensure_cache_dir(profile_dir);
    const std::string user_installation =
        std::string("-env:UserInstallation=file://") + profile_dir;

    std::vector<std::string> argv = {
        soffice,
        user_installation,
        "--headless",
        "--norestore",
        "--nologo",
        "--nolockcheck",
        "--nofirststartwizard",
        "--convert-to", target_ext,
        "--outdir", work_dir,
        path,
    };
    std::string log;
    std::lock_guard<std::mutex> lk(g_conv_mtx);
    const int rc = run_soffice(argv, log);
    // The convert produces `<work_dir>/<stem>.<target_ext>` regardless
    // of the source extension. Compute that name to report success.
    const std::string produced =
        work_dir + "/" + stem_of(path) + "." + target_ext;
    if (rc != 0) {
        warn("soffice convert-to " + target_ext + " returned " +
             std::to_string(rc) + " for " + path + "\n  log tail: " +
             (log.size() > 2048
                  ? log.substr(log.size() - 2048)
                  : log));
        // Best-effort profile cleanup even on failure.
        fs::remove_all(profile_dir, ec);
        return {};
    }
    fs::remove_all(profile_dir, ec);
    if (!fs::is_regular_file(produced, ec)) {
        warn("soffice reported success but " + produced +
             " was not produced.\n  log tail: " +
             (log.size() > 2048
                  ? log.substr(log.size() - 2048)
                  : log));
        return {};
    }
    return produced;
}

}  // namespace

std::string soffice_path() {
    return env_or("AC9_SOFFICE",
        "/home/jwoods/work/collabora-prefix/opt/collaboraoffice/program/soffice");
}

std::string cache_dir() {
    return env_or("AC9_OFFICE_CACHE", "/tmp/ac9-office-cache");
}

bool is_office_ext(std::string_view ext) {
    static const std::array<std::string_view, 7> known{
        "odt", "ods", "odp", "odg", "docx", "xlsx", "pptx",
    };
    for (auto k : known) if (k == ext) return true;
    return false;
}

std::string read(const std::string & path) {
    const std::string abs = fs::absolute(path).string();
    const std::int64_t mt = mtime_ns_of(abs);
    if (mt == 0) {
        warn("read(): missing file " + abs);
        return {};
    }
    // Cache hit?
    {
        std::lock_guard<std::mutex> lk(g_cache_mtx);
        auto it = g_text_cache.find({abs, mt});
        if (it != g_text_cache.end()) return it->second;
    }
    const std::string produced = convert_via_soffice(abs, "txt", cache_dir());
    if (produced.empty()) return {};
    const std::string text = slurp(produced);
    if (text.empty()) {
        warn("read(): soffice produced " + produced +
             " but slurp returned empty");
    }
    {
        std::lock_guard<std::mutex> lk(g_cache_mtx);
        g_text_cache[{abs, mt}] = text;
    }
    return text;
}

bool write(const std::string & path, const std::string & plain_text) {
    const std::string ext = ext_of(path);
    const std::string target = convert_target(ext);
    if (target.empty()) {
        warn("write(): unsupported extension in path " + path);
        return false;
    }
    if (target == "txt") {
        return spit(path, plain_text);
    }
    // Stage the text under a unique working sub-directory so soffice's
    // outdir-based naming does not collide with a concurrent write.
    const std::string wd = cache_dir() + "/write-" + random_suffix();
    ensure_cache_dir(wd);
    const std::string tmp_txt = wd + "/source.txt";
    if (!spit(tmp_txt, plain_text)) {
        warn("write(): could not stage tmp .txt at " + tmp_txt);
        return false;
    }
    const std::string produced = convert_via_soffice(tmp_txt, target, wd);
    std::error_code ec;
    if (produced.empty()) {
        fs::remove_all(wd, ec);
        return false;
    }
    // Ensure destination directory exists then move the produced file
    // into place. rename() first (same-fs common case), fall back to
    // copy+delete when crossing filesystems.
    fs::create_directories(fs::path(path).parent_path(), ec);
    fs::remove(path, ec);
    fs::rename(produced, path, ec);
    if (ec) {
        std::error_code cc;
        fs::copy_file(produced, path,
                      fs::copy_options::overwrite_existing, cc);
        if (cc) {
            warn("write(): could not land produced doc at " + path +
                 ": " + cc.message());
            fs::remove_all(wd, ec);
            return false;
        }
    }
    fs::remove_all(wd, ec);
    // Invalidate the read cache: new mtime, but stale entries in the
    // (path, mtime) map are harmless -- they simply never match again.
    return true;
}

bool edit(const std::string & path, const std::vector<Patch> & patches) {
    std::fprintf(stderr,
        "!!!! OFFICE DOC EDIT !!!! path=%s patches=%zu\n",
        path.c_str(), patches.size());
    std::string body = read(path);
    if (body.empty()) {
        warn("edit(): could not read " + path + " for patching");
        return false;
    }
    std::size_t total = 0;
    for (const auto & p : patches) {
        total += apply_patch(body, p);
    }
    std::fprintf(stderr,
        "!!!! OFFICE DOC EDIT !!!! applied %zu substitutions to %s\n",
        total, path.c_str());
    return write(path, body);
}

std::string summarize(const std::string & path) {
    const std::string body = read(path);
    if (body.empty()) return {};
    // Keep the prompt short so the small coder stays responsive: cap
    // the input at 4000 chars (~1000 tokens); the summary quality
    // survives truncation for anything longer.
    std::string clipped = body;
    if (clipped.size() > 4000) {
        clipped.resize(4000);
        clipped += "\n[...truncated...]";
    }
    std::string sys =
        "You are a technical summarizer. Read the plain-text extract "
        "of an Office document and produce a THREE-sentence summary. "
        "First sentence: what kind of document + main topic. Second "
        "sentence: key facts / numbers / names. Third sentence: "
        "concrete outcome or next step called out by the document. "
        "Never use an em dash (U+2014), en dash (U+2013), or horizontal "
        "bar (U+2015). Use a plain hyphen when a dash is needed. "
        "Output ONLY the three sentences.";
    std::string user = "Document extract:\n\n" + clipped;
    try {
        return coder::generate(sys, user, /*max_new_tokens=*/384, nullptr);
    } catch (const std::exception & ex) {
        warn(std::string("summarize(): coder threw: ") + ex.what());
        return {};
    }
}

json structure(const std::string & path) {
    const std::string ext = ext_of(path);
    if (!is_office_ext(ext)) {
        return {{"kind", "unknown"},
                {"error", "unsupported extension: " + ext}};
    }
    const std::string produced =
        convert_via_soffice(fs::absolute(path).string(), "html", cache_dir());
    if (produced.empty()) {
        return {{"kind", "unknown"},
                {"error", "soffice html conversion failed for " + path}};
    }
    const std::string html = slurp(produced);
    if (html.empty()) {
        return {{"kind", "unknown"},
                {"error", "empty html output at " + produced}};
    }
    json out;
    if (ext == "ods" || ext == "xlsx") {
        out["kind"]   = "spreadsheet";
        out["sheets"] = sheets_from_html(html);
    } else if (ext == "odp" || ext == "pptx") {
        out["kind"]   = "slides";
        out["slides"] = slides_from_html(html);
    } else if (ext == "odt" || ext == "docx") {
        out["kind"]     = "document";
        out["headings"] = headings_from_html(html);
    } else if (ext == "odg") {
        // Drawings have no natural table of contents; count <h1>-style
        // section markers so the LLM sees at least page count.
        const auto h = headings_from_html(html);
        out["kind"]  = "drawing";
        out["pages"] = h.size();
    }
    return out;
}

std::string sheet_read(const std::string & path,
                       const std::string & sheet_name) {
    // soffice --convert-to csv only exports the currently-active sheet
    // for .ods, or the first for the default filter. Callers that want
    // a specific tab need a filter-with-options, so we build one when
    // sheet_name is set. The filter name is the standard LibreOffice
    // Calc CSV filter; sheet_name is only used as a display hint at
    // this layer (the CSV filter honours "Active Sheet" by default).
    // Enough to give the LLM a scan of the primary sheet; when we grow
    // multi-tab needs we swap in a --headless macro pass.
    (void)sheet_name;
    const std::string produced = convert_via_soffice(
        fs::absolute(path).string(), "csv", cache_dir());
    if (produced.empty()) return {};
    return slurp(produced);
}

bool sheet_write(const std::string & path,
                 const std::string & csv,
                 const std::string & sheet_name) {
    (void)sheet_name;   // See sheet_read note above.
    const std::string ext = ext_of(path);
    if (ext != "ods" && ext != "xlsx" && ext != "csv") {
        warn("sheet_write(): unsupported target extension " + ext);
        return false;
    }
    if (ext == "csv") return spit(path, csv);
    // Stage the CSV, convert to the target sheet format, land in place.
    const std::string wd = cache_dir() + "/sheet-" + random_suffix();
    ensure_cache_dir(wd);
    const std::string tmp_csv = wd + "/source.csv";
    if (!spit(tmp_csv, csv)) {
        warn("sheet_write(): staging csv failed: " + tmp_csv);
        return false;
    }
    const std::string produced = convert_via_soffice(tmp_csv, ext, wd);
    std::error_code ec;
    if (produced.empty()) {
        fs::remove_all(wd, ec);
        return false;
    }
    fs::create_directories(fs::path(path).parent_path(), ec);
    fs::remove(path, ec);
    fs::rename(produced, path, ec);
    if (ec) {
        std::error_code cc;
        fs::copy_file(produced, path,
                      fs::copy_options::overwrite_existing, cc);
        if (cc) {
            warn("sheet_write(): could not land at " + path + ": " +
                 cc.message());
            fs::remove_all(wd, ec);
            return false;
        }
    }
    fs::remove_all(wd, ec);
    return true;
}

std::string slide_read(const std::string & path, int slide_num) {
    const std::string body = read(path);
    if (body.empty()) return {};
    if (slide_num <= 0) return body;
    // LibreOffice's txt export prints slides separated by two blank
    // lines. Split on that and hand back the requested 1-indexed slide.
    std::vector<std::string> chunks;
    std::string cur;
    int blanks = 0;
    for (char c : body) {
        cur.push_back(c);
        if (c == '\n') {
            ++blanks;
            if (blanks >= 2) {
                // Strip trailing blank lines before pushing.
                while (!cur.empty() &&
                       (cur.back() == '\n' || cur.back() == ' '))
                    cur.pop_back();
                if (!cur.empty()) chunks.push_back(std::move(cur));
                cur.clear();
                blanks = 0;
            }
        } else if (!std::isspace(static_cast<unsigned char>(c))) {
            blanks = 0;
        }
    }
    if (!cur.empty()) chunks.push_back(std::move(cur));
    if (slide_num < 1 || static_cast<std::size_t>(slide_num) > chunks.size()) {
        warn("slide_read(): slide_num " + std::to_string(slide_num) +
             " out of range (deck has " + std::to_string(chunks.size()) +
             " slides)");
        return {};
    }
    return chunks[static_cast<std::size_t>(slide_num - 1)];
}

bool slide_write(const std::string & path,
                 const std::vector<SlideDraft> & slides) {
    // Build a single .txt with each slide's title as a heading and the
    // body as bullet lines. soffice's --convert-to odp / pptx applies
    // the "Untitled" outline template, which turns the first non-empty
    // line into the slide title and subsequent lines into bullets.
    std::ostringstream out;
    for (std::size_t i = 0; i < slides.size(); ++i) {
        const auto & s = slides[i];
        if (i > 0) out << "\n\n";
        out << s.title << "\n";
        // Split body on newline; blank lines become slide breaks in
        // txt->odp, so we strip them.
        std::string body = s.body;
        std::string line;
        for (char c : body) {
            if (c == '\n') {
                if (!line.empty()) out << line << "\n";
                line.clear();
            } else {
                line.push_back(c);
            }
        }
        if (!line.empty()) out << line << "\n";
    }
    return write(path, out.str());
}

}  // namespace office_docs
