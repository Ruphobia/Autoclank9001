// SPDX-License-Identifier: GPL-3.0-or-later
#include "server.hpp"
#include "httplib.h"
#include "assets_gen.hpp"
#include "status.hpp"
#include "sessions_store.hpp"

// pipeline layers
#include "../001_prompt_cleanup/cleanup.hpp"
#include "../002_dictionary/dictionary.hpp"
#include "../003_stylize/stylize.hpp"
#include "../004_expertise/expertise.hpp"
#include "../005_context/context.hpp"
#include "../006_disambiguate/disambiguate.hpp"
#include "../007_knowledge/kb.hpp"
#include "../008_entities/entities.hpp"
#include "../009_tools/classify.hpp"
#include "../009_tools/answer.hpp"
#include "../009_tools/statement.hpp"
#include "../009_tools/shell/shell.hpp"
#include "../009_tools/shell/coder.hpp"
#include "../009_tools/physics/physics.hpp"
#include "../009_tools/chemistry/chemistry.hpp"
#include "../009_tools/vision/vision.hpp"
#include "../009_tools/components/components.hpp"
#include "../009_tools/websearch/websearch.hpp"
#include "../009_tools/websearch/project_cfg.hpp"
#include "../009_tools/planner/planner.hpp"
#include "../009_tools/image_resolver/image_resolver.hpp"
#include "../009_tools/tool_router/tool_router.hpp"
#include "../013_bench/bench.hpp"
#include "../012_hardware/hardware.hpp"

#include "../1624_image_generator/image_generator.hpp"
#include "../1620_advanced_raster_image_editor_photoshop_gimp_class/advanced_raster_image_editor_photoshop_gimp_class.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <condition_variable>
#include <unordered_map>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <sstream>
#include <chrono>
#include <functional>
#include <deque>
#include <regex>
#include <cstdint>
#include <string>
#include <csignal>
#include <ctime>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <unordered_set>
#include <vector>

namespace web_server {
namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace {

std::mutex                       g_mtx;
std::unique_ptr<httplib::Server> g_srv;
std::atomic<bool>                g_running{false};

// ---- helpers -----------------------------------------------------------

std::string expand_home(std::string p) {
    if (p.empty()) return p;
    if (p[0] == '~') {
        const char * home = std::getenv("HOME");
        return std::string(home ? home : "/") + p.substr(1);
    }
    return p;
}

const std::unordered_set<std::string> kStopWords = {
    "a","an","the","of","in","on","at","to","for","by","with","from","as",
    "and","or","but","so","if","when","while","because",
    "i","you","he","she","it","we","they","me","him","her","us","them",
    "is","am","are","was","were","be","been","being",
    "have","has","had","do","does","did","will","would","can","could","shall",
    "should","may","might","must","not","no","yes",
    "this","that","these","those","than",
};

std::vector<std::string> unique_words_in_order(const std::string & text) {
    std::vector<std::string> out;
    std::unordered_set<std::string> seen;
    std::string cur;
    auto flush = [&]{
        if (cur.empty()) return;
        std::string lower;
        lower.reserve(cur.size());
        for (char c : cur) lower.push_back(static_cast<char>(std::tolower((unsigned char)c)));
        if (seen.insert(lower).second) out.push_back(lower);
        cur.clear();
    };
    for (char c : text) {
        if (std::isalpha((unsigned char)c)) cur.push_back(c);
        else flush();
    }
    flush();
    return out;
}

std::string build_stylize_defs(const std::vector<std::string> & words) {
    // Bounded: unbounded WordNet dumps flooded the trail with 2-15KB of
    // definitions per turn. Three senses per word, each clipped, with a
    // total cap; stylize only needs enough to enumerate plausible senses.
    constexpr std::size_t kMaxSenses   = 4;
    constexpr std::size_t kSenseCap    = 220;
    constexpr std::size_t kTotalCap    = 4000;
    std::string s;
    bool first = true;
    for (const std::string & w : words) {
        if (kStopWords.count(w)) continue;
        if (s.size() > kTotalCap) { s.append(", ..."); break; }
        const auto entries = dictionary::lookup(w);
        std::vector<std::string> senses;
        for (const auto & e : entries) {
            if (e.source != "WordNet") continue;
            std::string d = e.definition;
            if (d.size() > kSenseCap) { d.resize(kSenseCap); d += "..."; }
            senses.push_back(std::move(d));
            if (senses.size() >= kMaxSenses) break;
        }
        if (senses.empty()) continue;
        if (!first) s.append(", ");
        first = false;
        s.append(w).append(" (");
        for (std::size_t i = 0; i < senses.size(); ++i) {
            if (i) s.append("; ");
            s.append(senses[i]);
        }
        s.append(")");
    }
    return s;
}

// ---- handlers ----------------------------------------------------------

void handle_status(const httplib::Request &, httplib::Response & res) {
    res.set_content(status::snapshot_json(), "application/json");
}

void handle_fs_list(const httplib::Request & req, httplib::Response & res) {
    std::string path = req.get_param_value("path");
    if (path.empty()) path = std::getenv("HOME") ? std::getenv("HOME") : "/";
    path = expand_home(path);

    fs::path p;
    std::error_code ec;
    p = fs::absolute(path, ec);
    if (ec || !fs::exists(p, ec) || !fs::is_directory(p, ec)) {
        res.status = 404;
        json err{{"error","not a directory"},{"path",path}};
        res.set_content(err.dump(), "application/json");
        return;
    }

    std::vector<fs::directory_entry> entries;
    for (const auto & e : fs::directory_iterator(p, fs::directory_options::skip_permission_denied, ec)) {
        entries.push_back(e);
    }
    std::sort(entries.begin(), entries.end(), [](const auto & a, const auto & b){
        const bool da = a.is_directory(), db = b.is_directory();
        if (da != db) return da;
        return a.path().filename().string() < b.path().filename().string();
    });

    json out;
    out["path"]   = p.string();
    out["parent"] = p.parent_path().string();
    out["entries"] = json::array();
    for (const auto & e : entries) {
        json ee;
        ee["name"]   = e.path().filename().string();
        ee["is_dir"] = e.is_directory();
        out["entries"].push_back(std::move(ee));
    }
    res.set_content(out.dump(), "application/json");
}

// GET /api/fs/raw?path=...  — stream the file as its native bytes with
// a sniffed MIME type. Used by the editor pane to embed binary files
// (PDFs, images) directly via iframe / <img> instead of reading them
// through the JSON /api/fs/read path.
void handle_fs_raw(const httplib::Request & req, httplib::Response & res) {
    const std::string path = expand_home(req.get_param_value("path"));
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) {
        res.status = 404;
        res.set_content("not a file", "text/plain");
        return;
    }
    std::ifstream f(path, std::ios::binary);
    std::stringstream ss; ss << f.rdbuf();
    std::string body = ss.str();
    // Sniff MIME from extension.
    std::string mime = "application/octet-stream";
    auto dot = path.find_last_of('.');
    if (dot != std::string::npos) {
        std::string ext = path.substr(dot + 1);
        for (char & c : ext) c = static_cast<char>(std::tolower((unsigned char)c));
        if      (ext == "pdf")  mime = "application/pdf";
        else if (ext == "png")  mime = "image/png";
        else if (ext == "jpg" || ext == "jpeg") mime = "image/jpeg";
        else if (ext == "gif")  mime = "image/gif";
        else if (ext == "svg")  mime = "image/svg+xml";
        else if (ext == "webp") mime = "image/webp";
        else if (ext == "ico")  mime = "image/x-icon";
        else if (ext == "mp3")  mime = "audio/mpeg";
        else if (ext == "wav")  mime = "audio/wav";
        else if (ext == "mp4")  mime = "video/mp4";
        else if (ext == "txt" || ext == "md") mime = "text/plain; charset=utf-8";
    }
    res.set_content(body, mime);
}

void handle_fs_read(const httplib::Request & req, httplib::Response & res) {
    std::string path = expand_home(req.get_param_value("path"));
    std::error_code ec;
    if (!fs::is_regular_file(path, ec)) {
        res.status = 404;
        res.set_content(R"({"error":"not a file"})", "application/json");
        return;
    }
    if (fs::file_size(path) > 5 * 1024 * 1024) {
        res.status = 413;
        res.set_content(R"X({"error":"file too large (>5MB)"})X", "application/json");
        return;
    }
    std::ifstream f(path, std::ios::binary);
    std::stringstream ss; ss << f.rdbuf();
    json out;
    out["path"]    = path;
    out["content"] = ss.str();
    // Stamp the mtime so the editor tab can watch for external changes to
    // this exact revision. Same units as /api/fs/stat + /api/fs/scan.
    // Sent as a string: the raw nanosecond count overflows JS Number.
    {
        const auto ft = fs::last_write_time(path, ec);
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            ft.time_since_epoch()).count();
        out["mtime_ns"] = std::to_string(static_cast<long long>(ns));
    }
    // Use replace error-handler so binary content (e.g. someone clicking a
    // PDF expecting text) returns a lossy JSON instead of throwing 500.
    res.set_content(out.dump(-1, ' ', /*ensure_ascii=*/false,
                             nlohmann::json::error_handler_t::replace),
                    "application/json");
}

void handle_fs_mkdir(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("path")) {
        res.status = 400;
        res.set_content(R"({"error":"missing path"})", "application/json");
        return;
    }
    std::error_code ec;
    fs::create_directories(expand_home(body["path"].get<std::string>()), ec);
    if (ec) {
        res.status = 500;
        json err{{"error", ec.message()}};
        res.set_content(err.dump(), "application/json");
        return;
    }
    res.set_content(R"({"ok":true})", "application/json");
}

void handle_fs_write(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("path") || !body.contains("content")) {
        res.status = 400;
        res.set_content(R"({"error":"missing path or content"})", "application/json");
        return;
    }
    const std::string p = expand_home(body["path"].get<std::string>());
    std::ofstream f(p, std::ios::binary | std::ios::trunc);
    if (!f) {
        res.status = 500;
        res.set_content(R"({"error":"open failed"})", "application/json");
        return;
    }
    const std::string c = body["content"].get<std::string>();
    f.write(c.data(), c.size());
    f.close();
    // Return the new mtime so the client can update its "last known mtime"
    // marker for this tab; otherwise the file-watcher would immediately
    // treat the just-written revision as an external change and try to
    // re-fetch it on the next poll.
    json out{{"ok", true}};
    std::error_code ec;
    const auto ft = fs::last_write_time(p, ec);
    if (!ec) {
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            ft.time_since_epoch()).count();
        out["mtime_ns"] = std::to_string(static_cast<long long>(ns));
    }
    res.set_content(out.dump(), "application/json");
}

// POST /api/fs/write_raw?path=...   body = raw bytes (any Content-Type)
// Used by the image editor to overwrite an image file with the canvas
// contents (PNG, etc.) without base64 framing.
void handle_fs_write_raw(const httplib::Request & req, httplib::Response & res) {
    std::string path = expand_home(req.get_param_value("path"));
    if (path.empty()) {
        res.status = 400;
        res.set_content(R"({"error":"missing path"})", "application/json");
        return;
    }
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) {
        res.status = 500;
        res.set_content(R"({"error":"open failed"})", "application/json");
        return;
    }
    f.write(req.body.data(), static_cast<std::streamsize>(req.body.size()));
    res.set_content(R"({"ok":true})", "application/json");
}

// POST /api/fs/rename {from: "/abs/path", to: "newname" | "/abs/path"}
// A bare `to` (no slash) renames within the same directory.
void handle_fs_rename(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("from") || !body.contains("to")) {
        res.status = 400;
        res.set_content(R"({"error":"missing from or to"})", "application/json");
        return;
    }
    const fs::path from = expand_home(body["from"].get<std::string>());
    std::string to_raw  = body["to"].get<std::string>();
    fs::path to = to_raw.find('/') == std::string::npos
        ? from.parent_path() / to_raw
        : fs::path(expand_home(to_raw));
    std::error_code ec;
    if (fs::exists(to, ec)) {
        res.status = 409;
        res.set_content(R"({"error":"target already exists"})", "application/json");
        return;
    }
    fs::rename(from, to, ec);
    if (ec) {
        res.status = 500;
        json err{{"error", ec.message()}};
        res.set_content(err.dump(), "application/json");
        return;
    }
    json ok{{"ok", true}, {"path", to.string()}};
    res.set_content(ok.dump(), "application/json");
}

void handle_fs_delete(const httplib::Request & req, httplib::Response & res) {
    std::string path = expand_home(req.get_param_value("path"));
    std::error_code ec;
    fs::remove_all(path, ec);
    if (ec) {
        res.status = 500;
        json err{{"error", ec.message()}};
        res.set_content(err.dump(), "application/json");
        return;
    }
    res.set_content(R"({"ok":true})", "application/json");
}

// Returns a monotonic mtime fingerprint (int nanoseconds since epoch) for
// a single file, plus its size. The client uses this to detect external
// changes to open editor tabs and reload them if the on-disk copy has
// moved forward and the tab is not dirty. Missing / non-file paths
// respond 200 with exists=false rather than 404: the poller treats
// non-existent files as "still gone" and clears the tab if the user
// deleted it externally.
void handle_fs_stat(const httplib::Request & req, httplib::Response & res) {
    const std::string path = expand_home(req.get_param_value("path"));
    std::error_code ec;
    json out;
    out["path"] = path;
    if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec)) {
        out["exists"]   = false;
        out["mtime_ns"] = "0";
        out["size"]     = 0;
        res.set_content(out.dump(), "application/json");
        return;
    }
    const auto ft = fs::last_write_time(path, ec);
    const auto sz = fs::file_size(path, ec);
    // file_time_type has an implementation-defined epoch, and on this
    // libstdc++ the nanosecond count doesn't fit in a JS Number (2^53).
    // Send it as a string so the client compares it EXACTLY for equality
    // instead of silently truncating past 15 digits.
    const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        ft.time_since_epoch()).count();
    out["exists"]   = true;
    out["mtime_ns"] = std::to_string(static_cast<long long>(ns));
    out["size"]     = static_cast<int64_t>(sz);
    res.set_content(out.dump(), "application/json");
}

// Batch version of stat: POST {"paths":[...]} -> {"entries":[{path, exists,
// mtime_ns, size}, ...]}. Cheaper than N round-trips when the client is
// polling every open tab plus a subtree summary.
void handle_fs_scan(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("paths") ||
        !body["paths"].is_array()) {
        res.status = 400;
        res.set_content(R"({"error":"missing paths[]"})", "application/json");
        return;
    }
    json out;
    out["entries"] = json::array();
    for (const auto & pv : body["paths"]) {
        if (!pv.is_string()) continue;
        const std::string path = expand_home(pv.get<std::string>());
        std::error_code ec;
        json ee;
        ee["path"] = path;
        if (!fs::exists(path, ec) || !fs::is_regular_file(path, ec)) {
            ee["exists"]   = false;
            ee["mtime_ns"] = "0";
            ee["size"]     = 0;
        } else {
            const auto ft = fs::last_write_time(path, ec);
            const auto sz = fs::file_size(path, ec);
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                ft.time_since_epoch()).count();
            ee["exists"]   = true;
            ee["mtime_ns"] = std::to_string(static_cast<long long>(ns));
            ee["size"]     = static_cast<int64_t>(sz);
        }
        out["entries"].push_back(std::move(ee));
    }
    res.set_content(out.dump(), "application/json");
}

// Directory-tree fingerprint used to detect NEW or REMOVED files in the
// project. Client passes ?path=<root> and gets back one 64-bit hash that
// combines every entry name + its is_dir flag; if the value differs from
// the last poll, the tree needs a refresh. Recurses one level per hidden-
// aware directory walk, capped to 4096 entries so a stray gigabyte of
// build artifacts can't tar-pit the poll. Skips CMake / node_modules /
// .git which change constantly and would keep the tree refreshing.
void handle_fs_tree_hash(const httplib::Request & req, httplib::Response & res) {
    std::string path = req.get_param_value("path");
    if (path.empty()) { res.set_content(R"({"hash":"0"})", "application/json"); return; }
    path = expand_home(path);
    std::error_code ec;
    if (!fs::is_directory(path, ec)) {
        res.set_content(R"({"hash":"0"})", "application/json");
        return;
    }
    static const std::string kNoise[] = {
        "CMakeFiles", "CMakeCache.txt", "cmake_install.cmake",
        ".git", "node_modules", "build", "_deps", "__pycache__",
    };
    std::uint64_t h = 1469598103934665603ull;      // FNV-1a offset basis
    auto mix = [&h](std::string_view s) {
        for (unsigned char c : s) { h ^= c; h *= 1099511628211ull; }
        h ^= '\n'; h *= 1099511628211ull;
    };
    std::size_t counted = 0;
    fs::recursive_directory_iterator it(path,
        fs::directory_options::skip_permission_denied, ec), end;
    for (; it != end && !ec; it.increment(ec)) {
        if (counted > 4096) break;
        const std::string name = it->path().filename().string();
        bool skip = false;
        for (const auto & noisy : kNoise) if (name == noisy) { skip = true; break; }
        if (skip) { if (it->is_directory(ec)) it.disable_recursion_pending(); continue; }
        const std::string rel = fs::relative(it->path(), path, ec).string();
        mix(rel);
        mix(it->is_directory(ec) ? "d" : "f");
        if (!it->is_directory(ec)) {
            const auto ft = fs::last_write_time(it->path(), ec);
            const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                ft.time_since_epoch()).count();
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%lld",
                          static_cast<long long>(ns));
            mix(buf);
        }
        ++counted;
    }
    json out;
    out["hash"] = std::to_string(h);
    out["count"] = static_cast<int64_t>(counted);
    res.set_content(out.dump(), "application/json");
}

// Single-quote a string for safe embedding in a /bin/sh command line.
std::string term_squote(const std::string & s) {
    std::string o = "'";
    for (char c : s) { if (c == '\'') o += "'\\''"; else o += c; }
    o += "'";
    return o;
}

// Bash preamble that mirrors Ubuntu's interactive colour defaults so the
// web terminal looks like a normal shell: TERM set, dircolors loaded, and
// the standard --color=auto aliases. With a real TTY on stdout these emit
// ANSI colour; piped into another command they correctly stay plain.
const char * kTermColorPreamble =
    "export TERM=xterm-256color\n"
    "shopt -s expand_aliases\n"
    "if command -v dircolors >/dev/null 2>&1; then eval \"$(dircolors -b 2>/dev/null)\"; fi\n"
    "alias ls='ls --color=auto' dir='dir --color=auto' vdir='vdir --color=auto' "
    "grep='grep --color=auto' fgrep='fgrep --color=auto' egrep='egrep --color=auto' "
    "ip='ip --color=auto' 2>/dev/null\n";

// Run `script` under a pseudo-terminal so colour-on-tty tools behave as in
// a real terminal. Returns false (leaving out/exit_code untouched) if a PTY
// could not be allocated, so the caller can fall back to a plain pipe.
bool run_terminal_pty(const std::string & script, int cols, int rows,
                      std::string & out, int & exit_code) {
    int master = ::posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0) return false;
    if (::grantpt(master) != 0 || ::unlockpt(master) != 0) { ::close(master); return false; }
    const char * sname = ::ptsname(master);
    if (!sname) { ::close(master); return false; }
    std::string slave_name = sname;

    winsize ws{};
    ws.ws_row = static_cast<unsigned short>(rows > 0 ? rows : 40);
    ws.ws_col = static_cast<unsigned short>(cols > 0 ? cols : 80);
    ::ioctl(master, TIOCSWINSZ, &ws);

    const pid_t pid = ::fork();
    if (pid < 0) { ::close(master); return false; }
    if (pid == 0) {
        // Child: new session, slave becomes controlling tty on stdout/stderr;
        // stdin is /dev/null so a command that reads input gets EOF, not hang.
        ::setsid();
        const int slave = ::open(slave_name.c_str(), O_RDWR);
        if (slave < 0) _exit(127);
        ::ioctl(slave, TIOCSCTTY, 0);
        const int devnull = ::open("/dev/null", O_RDONLY);
        if (devnull >= 0) ::dup2(devnull, 0);
        ::dup2(slave, 1);
        ::dup2(slave, 2);
        ::close(master);
        if (slave > 2) ::close(slave);
        if (devnull > 2) ::close(devnull);
        ::execl("/bin/bash", "bash", "-c", script.c_str(), static_cast<char *>(nullptr));
        _exit(127);
    }

    // Parent: read the master until the child exits (slave close -> EIO/EOF),
    // with a wall-clock cap so a runaway command can't wedge the request.
    constexpr int kMaxSeconds = 45;
    time_t start = ::time(nullptr);
    std::array<char, 4096> buf;
    bool killed = false;
    while (true) {
        pollfd pfd{ master, POLLIN, 0 };
        const int pr = ::poll(&pfd, 1, 2000);
        if (pr > 0 && (pfd.revents & (POLLIN | POLLHUP | POLLERR))) {
            const ssize_t n = ::read(master, buf.data(), buf.size());
            if (n > 0) { out.append(buf.data(), static_cast<std::size_t>(n)); }
            else break;               // 0 = EOF, <0 = EIO after slave closed
        }
        if (::time(nullptr) - start > kMaxSeconds) {
            ::kill(pid, SIGKILL);
            out.append("\n[terminal: command timed out after " +
                       std::to_string(kMaxSeconds) + "s]\n");
            killed = true;
            break;
        }
    }
    ::close(master);

    int status = 0;
    ::waitpid(pid, &status, 0);
    if (killed)                       exit_code = 124;
    else if (WIFEXITED(status))       exit_code = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))     exit_code = 128 + WTERMSIG(status);
    else                              exit_code = -1;
    return true;
}

// POST /api/terminal/exec  {cwd, command, cols?, rows?}
void handle_terminal_exec(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("command")) {
        res.status = 400;
        res.set_content(R"({"error":"missing command"})", "application/json");
        return;
    }
    std::string cmd = body["command"].get<std::string>();
    std::string cwd = expand_home(body.value("cwd", std::string{}));
    if (cwd.empty()) cwd = std::getenv("HOME") ? std::getenv("HOME") : "/";
    const int cols = body.value("cols", 80);
    const int rows = body.value("rows", 40);

    const std::string script = std::string(kTermColorPreamble) +
        "cd " + term_squote(cwd) + " && ( " + cmd + " )";

    std::string out_buf;
    int exit_code = -1;
    if (!run_terminal_pty(script, cols, rows, out_buf, exit_code)) {
        // Fallback: plain pipe (no colour) if PTY allocation failed.
        std::string shell_cmd = "cd " + term_squote(cwd) + " && (" + cmd + ") 2>&1";
        if (FILE * pipe = ::popen(shell_cmd.c_str(), "r")) {
            std::array<char, 4096> buf;
            while (std::size_t n = std::fread(buf.data(), 1, buf.size(), pipe)) {
                out_buf.append(buf.data(), n);
            }
            const int status_code = ::pclose(pipe);
            if (status_code != -1 && WIFEXITED(status_code)) exit_code = WEXITSTATUS(status_code);
            else if (status_code != -1 && WIFSIGNALED(status_code)) exit_code = 128 + WTERMSIG(status_code);
        } else {
            res.status = 500;
            res.set_content(R"({"error":"exec failed"})", "application/json");
            return;
        }
    }

    json out;
    out["command"]   = cmd;
    out["cwd"]       = cwd;
    out["stdout"]    = out_buf;
    out["exit_code"] = exit_code;
    res.set_content(out.dump(), "application/json");
}

// POST /api/terminal/exec_stream  {cwd, command, cols?, rows?}
//
// Streaming variant of exec: allocates a PTY, forks bash, streams every
// read from the master back to the client via SSE as it happens. The
// child keeps running until it exits on its own or the client aborts the
// fetch (at which point the server's next sink.write fails and the child
// gets SIGKILL). No 45-second wall clock; the browser tab close is the
// natural terminator for a long-running program like ./quantiprize or
// `watch nvidia-smi` where the old buffer-and-return model made the
// terminal look dead until the timeout fired.
//
// Events:
//   event: chunk   data: {"data":"<raw bytes as a JSON string>"}
//   event: exit    data: {"code":N}
//   event: error   data: {"error":"..."}
void handle_terminal_exec_stream(const httplib::Request & req,
                                 httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("command")) {
        res.status = 400;
        res.set_content(R"({"error":"missing command"})", "application/json");
        return;
    }
    std::string cmd = body["command"].get<std::string>();
    std::string cwd = expand_home(body.value("cwd", std::string{}));
    if (cwd.empty()) cwd = std::getenv("HOME") ? std::getenv("HOME") : "/";
    const int cols = body.value("cols", 80);
    const int rows = body.value("rows", 40);

    const std::string script = std::string(kTermColorPreamble) +
        "cd " + term_squote(cwd) + " && ( " + cmd + " )";

    res.set_chunked_content_provider(
        "text/event-stream",
        [script, cols, rows](std::size_t, httplib::DataSink & sink) -> bool {
            auto emit = [&](const char * evt, const std::string & payload) -> bool {
                std::string s = "event: ";
                s.append(evt).append("\ndata: ").append(payload).append("\n\n");
                return sink.write(s.data(), s.size());
            };
            auto emit_error = [&](const std::string & msg) {
                json j{{"error", msg}};
                emit("error", j.dump());
                sink.done();
            };

            int master = ::posix_openpt(O_RDWR | O_NOCTTY);
            if (master < 0) { emit_error("posix_openpt failed"); return false; }
            if (::grantpt(master) != 0 || ::unlockpt(master) != 0) {
                ::close(master); emit_error("grantpt/unlockpt failed"); return false;
            }
            const char * sname = ::ptsname(master);
            if (!sname) { ::close(master); emit_error("ptsname failed"); return false; }
            const std::string slave_name = sname;

            winsize ws{};
            ws.ws_row = static_cast<unsigned short>(rows > 0 ? rows : 40);
            ws.ws_col = static_cast<unsigned short>(cols > 0 ? cols : 80);
            ::ioctl(master, TIOCSWINSZ, &ws);

            const pid_t pid = ::fork();
            if (pid < 0) { ::close(master); emit_error("fork failed"); return false; }
            if (pid == 0) {
                ::setsid();
                const int slave = ::open(slave_name.c_str(), O_RDWR);
                if (slave < 0) _exit(127);
                ::ioctl(slave, TIOCSCTTY, 0);
                // Give the child a REAL tty on stdin too, so programs that
                // check `isatty(0)` (top, watch's SIGWINCH handling, etc.)
                // see one. Because we still don't wire a keyboard, keystrokes
                // in the browser don't reach the child yet -- that will come
                // when the terminal gets a full-duplex WebSocket-shaped
                // channel. But even one-way stdin=tty unblocks tools that
                // just probed for a tty and quit early otherwise.
                ::dup2(slave, 0);
                ::dup2(slave, 1);
                ::dup2(slave, 2);
                ::close(master);
                if (slave > 2) ::close(slave);
                ::execl("/bin/bash", "bash", "-c", script.c_str(),
                        static_cast<char *>(nullptr));
                _exit(127);
            }

            // Parent: pump master output as SSE chunks. `sink.write` returns
            // false when the client disconnected; that becomes our SIGKILL
            // signal so a runaway server the user closed the tab on doesn't
            // linger.
            std::array<char, 4096> buf;
            int exit_code = -1;
            bool client_alive = true;
            while (true) {
                pollfd pfd{ master, POLLIN, 0 };
                const int pr = ::poll(&pfd, 1, 1000);
                if (pr > 0 && (pfd.revents & (POLLIN | POLLHUP | POLLERR))) {
                    const ssize_t n = ::read(master, buf.data(), buf.size());
                    if (n > 0) {
                        std::string data(buf.data(), static_cast<std::size_t>(n));
                        json j{{"data", data}};
                        const std::string payload =
                            j.dump(-1, ' ', /*ensure_ascii=*/false,
                                   nlohmann::json::error_handler_t::replace);
                        if (!emit("chunk", payload)) {
                            std::fprintf(stderr,
                                "[term_stream pid=%d] chunk emit failed; "
                                "assuming client gone\n", (int) pid);
                            client_alive = false;
                            ::kill(-pid, SIGKILL);
                            ::kill( pid, SIGKILL);
                            break;
                        }
                    } else {
                        std::fprintf(stderr,
                            "[term_stream pid=%d] master read returned %zd "
                            "(errno=%d); breaking\n",
                            (int) pid, n, errno);
                        break;
                    }
                }
                // Poll for child exit even when the pty went quiet, so a
                // silent-then-exit command (e.g. `sleep 5 && true`) doesn't
                // sit forever in select.
                int wstatus = 0;
                const pid_t r = ::waitpid(pid, &wstatus, WNOHANG);
                if (r == pid) {
                    if (WIFEXITED(wstatus))       exit_code = WEXITSTATUS(wstatus);
                    else if (WIFSIGNALED(wstatus)) exit_code = 128 + WTERMSIG(wstatus);
                    std::fprintf(stderr,
                        "[term_stream pid=%d] child exited on its own, "
                        "wstatus=0x%x exit_code=%d\n",
                        (int) pid, wstatus, exit_code);
                    break;
                }
                // Detect client disconnect between chunks. When bash is
                // idling inside a `sleep 60` the master produces nothing
                // and sink.write is never attempted, so a browser tab
                // closed halfway through would leak the child. We ACTIVELY
                // test the socket by writing an SSE keepalive comment; if
                // the write fails, the peer is gone. Comments (lines that
                // start with `:`) are ignored by EventSource clients per
                // the SSE spec, so the browser-side terminal never sees
                // them. Only send the keepalive every ~5 seconds so we
                // don't stress the socket during noisy PTY streams.
                static thread_local int keepalive_tick = 0;
                if (++keepalive_tick >= 5) {
                    keepalive_tick = 0;
                    const char ping[] = ": keepalive\n\n";
                    if (!sink.write(ping, sizeof(ping) - 1)) {
                        std::fprintf(stderr,
                            "[term_stream pid=%d] keepalive write failed; "
                            "client disconnected\n", (int) pid);
                        client_alive = false;
                        ::kill(-pid, SIGKILL);
                        ::kill( pid, SIGKILL);
                        break;
                    }
                }
            }
            ::close(master);

            // If we broke on EOF/EIO rather than a waitpid hit, reap now so
            // we get a real exit code before we tell the client we're done.
            if (exit_code < 0 && client_alive) {
                int wstatus = 0;
                ::waitpid(pid, &wstatus, 0);
                if (WIFEXITED(wstatus))       exit_code = WEXITSTATUS(wstatus);
                else if (WIFSIGNALED(wstatus)) exit_code = 128 + WTERMSIG(wstatus);
            } else if (!client_alive) {
                // Child was SIGKILL'd; drain the zombie.
                int wstatus = 0;
                ::waitpid(pid, &wstatus, 0);
                (void) wstatus;
                sink.done();
                return false;
            }
            json j{{"code", exit_code}};
            emit("exit", j.dump());
            sink.done();
            return false;
        });
}

// ===================================================================
// Persistent PTY sessions -- the "real terminal" path.
//
// A tab in the browser is one long-lived bash session behind an xterm.js
// emulator. The client opens a session, subscribes to its output stream,
// posts keystrokes back verbatim, and TIOCSWINSZ-resizes when its font
// metrics change. Ctrl-C / Ctrl-D / arrow keys / tab completion all work
// naturally: kernel PTY line discipline translates \x03 into SIGINT for
// the pgid, bash handles backspace, cooked mode echoes, etc.
//
// Endpoints:
//   POST /api/terminal/open   {cwd, cols, rows}         -> {tid, pid}
//   GET  /api/terminal/stream ?tid=...                  -> SSE bytes
//   POST /api/terminal/write  {tid, data (base64)}      -> {ok}
//   POST /api/terminal/resize {tid, cols, rows}         -> {ok}
//   POST /api/terminal/close  {tid}                     -> {ok}
//
// The stream endpoint is one-shot per subscription -- the client can
// resubscribe after a reload; the PTY survives. A rolling scrollback of
// 128 KB is kept per session so a reconnect replays recent output.
// Sessions that go >30 minutes with no active stream get reaped.
// ===================================================================
namespace {

struct PtyTab {
    int         master_fd = -1;
    pid_t       pid       = -1;
    std::string cwd;
    int         cols = 80, rows = 40;
    std::mutex  buf_mu;
    std::string scrollback;                // rolling, capped at kScrollbackCap
    std::mutex  wake_mu;
    std::condition_variable wake;
    std::atomic<uint64_t>   seq{0};        // bumped every append; readers wait on this
    std::atomic<bool>       alive{true};
    std::atomic<int64_t>    last_touch{0}; // seconds-since-epoch, for GC
    std::thread             reader;
};
constexpr std::size_t kScrollbackCap = 128 * 1024;

std::mutex                                         g_tabs_mu;
std::unordered_map<std::string, std::shared_ptr<PtyTab>> g_tabs;

std::string gen_tid() {
    static std::atomic<uint64_t> n{0};
    char buf[24];
    std::snprintf(buf, sizeof(buf), "t%llu-%llx",
                  static_cast<unsigned long long>(++n),
                  static_cast<unsigned long long>(
                      std::chrono::steady_clock::now().time_since_epoch().count()));
    return buf;
}
int64_t now_secs() {
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch()).count();
}

// Reader thread: read master forever, append to scrollback, wake stream subscribers.
// Uses poll() instead of select() so it does not abort with an
// "fd out of range" error when the process has accumulated more than
// FD_SETSIZE (1024) open descriptors -- a busy tool + a couple of PTY
// tabs + the periodic client polls (fs/scan, fs/tree_hash, terminal
// stream, chat SSE) trip that fast.
void pty_reader_loop(std::shared_ptr<PtyTab> tab) {
    std::array<char, 8192> buf;
    while (tab->alive.load()) {
        pollfd pfd{ tab->master_fd, POLLIN, 0 };
        const int pr = ::poll(&pfd, 1, 1000);
        if (pr > 0 && (pfd.revents & (POLLIN | POLLHUP | POLLERR))) {
            const ssize_t n = ::read(tab->master_fd, buf.data(), buf.size());
            if (n > 0) {
                {
                    std::lock_guard<std::mutex> lk(tab->buf_mu);
                    tab->scrollback.append(buf.data(), static_cast<std::size_t>(n));
                    if (tab->scrollback.size() > kScrollbackCap) {
                        tab->scrollback.erase(
                            0, tab->scrollback.size() - kScrollbackCap);
                    }
                }
                tab->seq.fetch_add(1, std::memory_order_release);
                tab->wake.notify_all();
            } else {
                break;
            }
        }
        // Poll for child exit even during quiet periods; a bash that
        // received exit / Ctrl-D should tear the session down.
        int wstatus = 0;
        const pid_t r = ::waitpid(tab->pid, &wstatus, WNOHANG);
        if (r == tab->pid) break;
    }
    tab->alive.store(false);
    tab->seq.fetch_add(1, std::memory_order_release);
    tab->wake.notify_all();
}

// Best-effort cleanup: send SIGHUP to the pgid, close master, reap.
void pty_close(const std::shared_ptr<PtyTab> & tab) {
    if (!tab) return;
    tab->alive.store(false);
    if (tab->pid > 0) {
        ::kill(-tab->pid, SIGHUP);
        ::kill( tab->pid, SIGHUP);
    }
    if (tab->master_fd >= 0) {
        ::close(tab->master_fd);
        tab->master_fd = -1;
    }
    if (tab->pid > 0) {
        int st = 0;
        // Non-blocking reap first; then a bounded blocking wait.
        for (int i = 0; i < 10; ++i) {
            const pid_t r = ::waitpid(tab->pid, &st, WNOHANG);
            if (r == tab->pid || r < 0) { tab->pid = -1; break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        if (tab->pid > 0) {
            ::kill(-tab->pid, SIGKILL);
            ::kill( tab->pid, SIGKILL);
            ::waitpid(tab->pid, &st, 0);
            tab->pid = -1;
        }
    }
    if (tab->reader.joinable()) tab->reader.join();
}

void gc_stale_tabs() {
    const int64_t cutoff = now_secs() - 30 * 60;
    std::vector<std::shared_ptr<PtyTab>> reap;
    {
        std::lock_guard<std::mutex> lk(g_tabs_mu);
        for (auto it = g_tabs.begin(); it != g_tabs.end();) {
            if (it->second->last_touch.load() < cutoff ||
                !it->second->alive.load()) {
                reap.push_back(it->second);
                it = g_tabs.erase(it);
            } else ++it;
        }
    }
    for (auto & t : reap) pty_close(t);
}

// POST /api/terminal/open {cwd, cols, rows} -> {tid, pid}
void handle_terminal_open(const httplib::Request & req, httplib::Response & res) {
    gc_stale_tabs();
    json body = json::parse(req.body, nullptr, false);
    std::string cwd = expand_home(body.value("cwd", std::string{}));
    if (cwd.empty()) cwd = std::getenv("HOME") ? std::getenv("HOME") : "/";
    const int cols = body.value("cols", 80);
    const int rows = body.value("rows", 40);

    int master = ::posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0) {
        res.status = 500;
        res.set_content(R"({"error":"posix_openpt failed"})", "application/json");
        return;
    }
    if (::grantpt(master) != 0 || ::unlockpt(master) != 0) {
        ::close(master);
        res.status = 500;
        res.set_content(R"({"error":"grantpt/unlockpt failed"})", "application/json");
        return;
    }
    const char * sname = ::ptsname(master);
    if (!sname) {
        ::close(master);
        res.status = 500;
        res.set_content(R"({"error":"ptsname failed"})", "application/json");
        return;
    }
    const std::string slave_name = sname;
    winsize ws{};
    ws.ws_row = static_cast<unsigned short>(rows > 0 ? rows : 40);
    ws.ws_col = static_cast<unsigned short>(cols > 0 ? cols : 80);
    ::ioctl(master, TIOCSWINSZ, &ws);

    const pid_t pid = ::fork();
    if (pid < 0) {
        ::close(master);
        res.status = 500;
        res.set_content(R"({"error":"fork failed"})", "application/json");
        return;
    }
    if (pid == 0) {
        // Child: new session with slave as controlling tty, launch an
        // interactive login-ish bash so the user gets .bashrc, aliases,
        // prompt, etc. Use --rcfile fallback if HOME/.bashrc missing.
        ::setsid();
        const int slave = ::open(slave_name.c_str(), O_RDWR);
        if (slave < 0) _exit(127);
        ::ioctl(slave, TIOCSCTTY, 0);
        ::dup2(slave, 0);
        ::dup2(slave, 1);
        ::dup2(slave, 2);
        ::close(master);
        if (slave > 2) ::close(slave);
        // Move into the requested cwd (chdir failure is non-fatal;
        // bash will just start in the parent's cwd instead).
        if (!cwd.empty()) { int r_ = ::chdir(cwd.c_str()); (void) r_; }
        ::setenv("TERM", "xterm-256color", 1);
        // -i for interactive (so bash reads bashrc + shows prompt).
        ::execl("/bin/bash", "bash", "-i", static_cast<char *>(nullptr));
        _exit(127);
    }

    auto tab = std::make_shared<PtyTab>();
    tab->master_fd = master;
    tab->pid       = pid;
    tab->cwd       = cwd;
    tab->cols      = cols;
    tab->rows      = rows;
    tab->last_touch.store(now_secs());
    tab->reader = std::thread(pty_reader_loop, tab);

    const std::string tid = gen_tid();
    {
        std::lock_guard<std::mutex> lk(g_tabs_mu);
        g_tabs.emplace(tid, tab);
    }
    json out{{"tid", tid}, {"pid", pid}};
    res.set_content(out.dump(), "application/json");
}

std::shared_ptr<PtyTab> find_tab(const std::string & tid) {
    std::lock_guard<std::mutex> lk(g_tabs_mu);
    auto it = g_tabs.find(tid);
    if (it == g_tabs.end()) return nullptr;
    it->second->last_touch.store(now_secs());
    return it->second;
}

// GET /api/terminal/stream?tid=...
// SSE stream of PTY output. Sends the scrollback tail on first
// connect, then live bytes as they arrive.
void handle_terminal_stream(const httplib::Request & req, httplib::Response & res) {
    const std::string tid = req.get_param_value("tid");
    auto tab = find_tab(tid);
    if (!tab) {
        res.status = 404;
        res.set_content(R"({"error":"unknown tid"})", "application/json");
        return;
    }
    res.set_chunked_content_provider(
        "text/event-stream",
        [tab](std::size_t, httplib::DataSink & sink) -> bool {
            auto emit = [&](const char * evt, const std::string & payload) -> bool {
                std::string s = "event: ";
                s.append(evt).append("\ndata: ").append(payload).append("\n\n");
                return sink.write(s.data(), s.size());
            };
            // Send the scrollback so a reload restores the visible screen.
            std::string tail;
            {
                std::lock_guard<std::mutex> lk(tab->buf_mu);
                tail = tab->scrollback;
            }
            if (!tail.empty()) {
                json j{{"data", tail}};
                emit("data", j.dump(-1, ' ', /*ensure_ascii=*/false,
                                    nlohmann::json::error_handler_t::replace));
            }
            uint64_t last_seq = tab->seq.load(std::memory_order_acquire);
            std::size_t sent_len = tail.size();
            while (tab->alive.load()) {
                // Wait up to 1s for new data (or a shutdown notification).
                {
                    std::unique_lock<std::mutex> lk(tab->wake_mu);
                    tab->wake.wait_for(lk, std::chrono::seconds(1),
                        [&]{ return tab->seq.load() != last_seq || !tab->alive.load(); });
                }
                std::string fresh;
                {
                    std::lock_guard<std::mutex> lk(tab->buf_mu);
                    // Emit whatever appended past the last sent index.
                    // scrollback may have wrapped -- if we lost the tail
                    // we started from, restart from the beginning of the
                    // current buffer.
                    if (sent_len <= tab->scrollback.size()) {
                        fresh = tab->scrollback.substr(sent_len);
                        sent_len = tab->scrollback.size();
                    } else {
                        fresh = tab->scrollback;
                        sent_len = tab->scrollback.size();
                    }
                }
                if (!fresh.empty()) {
                    json j{{"data", fresh}};
                    if (!emit("data",
                              j.dump(-1, ' ', /*ensure_ascii=*/false,
                                     nlohmann::json::error_handler_t::replace))) {
                        break;    // client disconnected
                    }
                } else {
                    // Idle tick: send an SSE comment keepalive; on failure
                    // we can safely tear this subscription down (the PTY
                    // stays alive for the next stream call).
                    const char ping[] = ": keepalive\n\n";
                    if (!sink.write(ping, sizeof(ping) - 1)) break;
                }
                last_seq = tab->seq.load(std::memory_order_acquire);
            }
            if (!tab->alive.load()) {
                emit("closed", R"({"code":0})");
            }
            sink.done();
            return false;
        });
}

// POST /api/terminal/write {tid, data}
// `data` is the raw string of bytes to inject into the master.
void handle_terminal_write(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("tid") || !body.contains("data")) {
        res.status = 400;
        res.set_content(R"({"error":"need {tid,data}"})", "application/json");
        return;
    }
    auto tab = find_tab(body["tid"].get<std::string>());
    if (!tab || !tab->alive.load()) {
        res.status = 404;
        res.set_content(R"({"error":"dead tid"})", "application/json");
        return;
    }
    const std::string data = body["data"].get<std::string>();
    ssize_t off = 0;
    while (off < static_cast<ssize_t>(data.size())) {
        const ssize_t n = ::write(tab->master_fd, data.data() + off,
                                  data.size() - off);
        if (n > 0) off += n;
        else if (n < 0 && (errno == EINTR || errno == EAGAIN)) continue;
        else break;
    }
    res.set_content(R"({"ok":true})", "application/json");
}

// POST /api/terminal/resize {tid, cols, rows}
void handle_terminal_resize(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    auto tab = find_tab(body.value("tid", std::string{}));
    if (!tab || !tab->alive.load()) {
        res.status = 404;
        res.set_content(R"({"error":"dead tid"})", "application/json");
        return;
    }
    tab->cols = body.value("cols", tab->cols);
    tab->rows = body.value("rows", tab->rows);
    winsize ws{};
    ws.ws_row = static_cast<unsigned short>(tab->rows);
    ws.ws_col = static_cast<unsigned short>(tab->cols);
    ::ioctl(tab->master_fd, TIOCSWINSZ, &ws);
    ::kill(-tab->pid, SIGWINCH);
    res.set_content(R"({"ok":true})", "application/json");
}

// POST /api/terminal/close {tid}
void handle_terminal_close(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    const std::string tid = body.value("tid", std::string{});
    std::shared_ptr<PtyTab> tab;
    {
        std::lock_guard<std::mutex> lk(g_tabs_mu);
        auto it = g_tabs.find(tid);
        if (it != g_tabs.end()) { tab = it->second; g_tabs.erase(it); }
    }
    if (tab) pty_close(tab);
    res.set_content(R"({"ok":true})", "application/json");
}

}  // anonymous namespace

// ===================================================================
// .tickets.agile board endpoints
// -------------------------------------------------------------------
// Per-project agile board persisted as pretty-printed JSON at
// <cwd>/.tickets.agile. A single global mutex serialises reads and
// writes so concurrent PATCH/move/delete requests never interleave
// their load-modify-save cycles. Saves are atomic (tmp+rename) so a
// crash mid-write can never corrupt the board file.
// ===================================================================
namespace {

std::mutex g_tickets_mtx;

std::string current_iso_utc() {
    using namespace std::chrono;
    const auto  now = system_clock::now();
    std::time_t t   = system_clock::to_time_t(now);
    std::tm     tm{};
    gmtime_r(&t, &tm);
    char buf[80];
    std::snprintf(buf, sizeof(buf),
                  "%04d-%02d-%02dT%02d:%02d:%02dZ",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buf);
}

fs::path tickets_path_for(const std::string & cwd) {
    return fs::path(expand_home(cwd)) / ".tickets.agile";
}

json default_board() {
    json b;
    b["version"] = 1;
    b["next_id"] = 1;
    b["columns"] = json::array({
        json{{"key", "todo"},    {"title", "To Do"}},
        json{{"key", "doing"},   {"title", "In Progress"}},
        json{{"key", "blocked"}, {"title", "Blocked"}},
        json{{"key", "done"},    {"title", "Done"}},
    });
    b["tickets"] = json::array();
    return b;
}

bool is_valid_priority(const std::string & p) {
    return p == "low" || p == "normal" || p == "high" || p == "urgent";
}

// The contract fixes `parent: "T-<int>" | null`. Anything else corrupts
// the board for readers that expect a typed record.
bool is_valid_parent_value(const json & v) {
    if (v.is_null()) return true;
    if (!v.is_string()) return false;
    static const std::regex parent_re(R"(^T-\d+$)");
    return std::regex_match(v.get<std::string>(), parent_re);
}

// labels: array of strings
bool is_valid_labels_value(const json & v) {
    if (!v.is_array()) return false;
    for (const auto & el : v) if (!el.is_string()) return false;
    return true;
}

std::unordered_set<std::string> column_keys_of(const json & board) {
    std::unordered_set<std::string> out;
    if (board.contains("columns") && board["columns"].is_array()) {
        for (const auto & c : board["columns"]) {
            if (c.is_object() && c.contains("key") && c["key"].is_string()) {
                out.insert(c["key"].get<std::string>());
            }
        }
    }
    return out;
}

// Load an existing board from disk. Returns false and sets err to
// "no board" if the file is missing; other errors get their own
// message. Caller must hold g_tickets_mtx.
bool load_board(const fs::path & p, json & out, std::string & err) {
    std::error_code ec;
    if (!fs::exists(p, ec) || !fs::is_regular_file(p, ec)) {
        err = "no board";
        return false;
    }
    std::ifstream f(p, std::ios::binary);
    if (!f.is_open()) { err = "open failed"; return false; }
    std::stringstream ss; ss << f.rdbuf();
    out = json::parse(ss.str(), nullptr, false);
    if (!out.is_object()) { err = "corrupt board"; return false; }
    return true;
}

// Atomic write: dump to <path>.tmp then rename(2) over the real file.
// rename is atomic on POSIX filesystems, so an interrupted write can
// only leave the .tmp behind, never a half-written .tickets.agile.
// Caller must hold g_tickets_mtx.
bool save_board_atomic(const fs::path & p, const json & board, std::string & err) {
    fs::path tmp = p;
    tmp += ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f.is_open()) { err = "open tmp failed"; return false; }
        f << board.dump(2);
        f.flush();
        if (!f) { err = "write failed"; return false; }
    }
    std::error_code ec;
    fs::rename(tmp, p, ec);
    if (ec) {
        std::error_code ec2;
        fs::remove(tmp, ec2);
        err = std::string("rename failed: ") + ec.message();
        return false;
    }
    return true;
}

void tickets_error(httplib::Response & res, int status, const std::string & msg) {
    res.status = status;
    res.set_content(json({{"error", msg}}).dump(), "application/json");
}

std::string ticket_id_from_path(const httplib::Request & req) {
    if (req.matches.size() >= 2) return req.matches[1].str();
    return {};
}

// -- handlers -----------------------------------------------------------

// GET /api/tickets?cwd=X
void handle_tickets_list(const httplib::Request & req, httplib::Response & res) {
    const std::string cwd = req.get_param_value("cwd");
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    res.set_content(board.dump(2), "application/json");
}

// POST /api/tickets/init {cwd}
void handle_tickets_init(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object()) { tickets_error(res, 400, "bad body"); return; }
    const std::string cwd = body.value("cwd", std::string{});
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    const fs::path p = tickets_path_for(cwd);
    std::error_code ec;
    json board;
    if (fs::exists(p, ec) && fs::is_regular_file(p, ec)) {
        std::string err;
        if (!load_board(p, board, err)) { tickets_error(res, 500, err); return; }
    } else {
        board = default_board();
        std::string err;
        if (!save_board_atomic(p, board, err)) { tickets_error(res, 500, err); return; }
    }
    res.set_content(board.dump(2), "application/json");
}

// POST /api/tickets/create {cwd, title, ...}
void handle_tickets_create(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object()) { tickets_error(res, 400, "bad body"); return; }
    const std::string cwd = body.value("cwd", std::string{});
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    const std::string title = body.value("title", std::string{});
    if (title.empty()) { tickets_error(res, 400, "empty title"); return; }

    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    const auto col_keys = column_keys_of(board);

    std::string status = body.value("status", std::string{});
    if (status.empty()) {
        // Default to first column key.
        if (board["columns"].is_array() && !board["columns"].empty()
            && board["columns"][0].is_object()
            && board["columns"][0].contains("key")) {
            status = board["columns"][0]["key"].get<std::string>();
        } else {
            status = "todo";
        }
    } else if (!col_keys.count(status)) {
        tickets_error(res, 400, "invalid status");
        return;
    }

    std::string priority = body.value("priority", std::string{"normal"});
    if (!is_valid_priority(priority)) {
        tickets_error(res, 400, "invalid priority");
        return;
    }

    const int next_id = board.value("next_id", 1);
    const std::string id = "T-" + std::to_string(next_id);
    const std::string now = current_iso_utc();

    json t;
    t["id"]       = id;
    t["title"]    = title;
    t["body"]     = body.value("body", std::string{});
    t["status"]   = status;
    if (body.contains("labels")) {
        if (!is_valid_labels_value(body["labels"])) {
            tickets_error(res, 400, "labels must be an array of strings");
            return;
        }
        t["labels"] = body["labels"];
    } else {
        t["labels"] = json::array();
    }
    t["priority"] = priority;
    t["created"]  = now;
    t["updated"]  = now;
    if (body.contains("parent")) {
        if (!is_valid_parent_value(body["parent"])) {
            tickets_error(res, 400, "parent must be null or a T-<n> id");
            return;
        }
        t["parent"] = body["parent"];
    } else {
        t["parent"] = nullptr;
    }
    t["extra"] = json::object();

    board["tickets"].push_back(t);
    board["next_id"] = next_id + 1;
    if (!save_board_atomic(tickets_path_for(cwd), board, err)) {
        tickets_error(res, 500, err);
        return;
    }
    res.set_content(t.dump(2), "application/json");
}

// GET /api/tickets/T-<n>?cwd=X
void handle_tickets_get(const httplib::Request & req, httplib::Response & res) {
    const std::string cwd = req.get_param_value("cwd");
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    const std::string id = ticket_id_from_path(req);

    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    if (board["tickets"].is_array()) {
        for (const auto & t : board["tickets"]) {
            if (t.value("id", std::string{}) == id) {
                res.set_content(t.dump(2), "application/json");
                return;
            }
        }
    }
    tickets_error(res, 404, "no ticket");
}

// PATCH /api/tickets/T-<n> {cwd, ...}
void handle_tickets_patch(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object()) { tickets_error(res, 400, "bad body"); return; }
    const std::string cwd = body.value("cwd", std::string{});
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    const std::string id = ticket_id_from_path(req);

    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    const auto col_keys = column_keys_of(board);

    json * target = nullptr;
    if (board["tickets"].is_array()) {
        for (auto & t : board["tickets"]) {
            if (t.value("id", std::string{}) == id) { target = &t; break; }
        }
    }
    if (!target) { tickets_error(res, 404, "no ticket"); return; }

    static const std::unordered_set<std::string> kPatchable = {
        "title", "body", "status", "labels", "priority", "parent", "extra"
    };
    for (auto it = body.begin(); it != body.end(); ++it) {
        const std::string & k = it.key();
        if (k == "cwd") continue;
        // id and created are immutable; unknown keys silently ignored.
        if (!kPatchable.count(k)) continue;
        const json & v = it.value();
        if (k == "title") {
            if (!v.is_string() || v.get<std::string>().empty()) {
                tickets_error(res, 400, "empty title");
                return;
            }
        } else if (k == "status") {
            if (!v.is_string() || !col_keys.count(v.get<std::string>())) {
                tickets_error(res, 400, "invalid status");
                return;
            }
        } else if (k == "priority") {
            if (!v.is_string() || !is_valid_priority(v.get<std::string>())) {
                tickets_error(res, 400, "invalid priority");
                return;
            }
        } else if (k == "labels") {
            if (!is_valid_labels_value(v)) {
                tickets_error(res, 400, "labels must be an array of strings");
                return;
            }
        } else if (k == "parent") {
            if (!is_valid_parent_value(v)) {
                tickets_error(res, 400, "parent must be null or a T-<n> id");
                return;
            }
        } else if (k == "extra") {
            if (!v.is_object()) {
                tickets_error(res, 400, "extra must be an object");
                return;
            }
        }
        (*target)[k] = v;
    }
    (*target)["updated"] = current_iso_utc();
    if (!save_board_atomic(tickets_path_for(cwd), board, err)) {
        tickets_error(res, 500, err);
        return;
    }
    res.set_content(target->dump(2), "application/json");
}

// DELETE /api/tickets/T-<n>?cwd=X
void handle_tickets_delete(const httplib::Request & req, httplib::Response & res) {
    const std::string cwd = req.get_param_value("cwd");
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    const std::string id = ticket_id_from_path(req);

    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    bool found = false;
    if (board["tickets"].is_array()) {
        auto & arr = board["tickets"];
        for (auto it = arr.begin(); it != arr.end(); ++it) {
            if (it->value("id", std::string{}) == id) {
                arr.erase(it);
                found = true;
                break;
            }
        }
    }
    if (!found) { tickets_error(res, 404, "no ticket"); return; }
    if (!save_board_atomic(tickets_path_for(cwd), board, err)) {
        tickets_error(res, 500, err);
        return;
    }
    res.set_content(R"({"ok":true})", "application/json");
}

// POST /api/tickets/T-<n>/move {cwd, status}
void handle_tickets_move(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object()) { tickets_error(res, 400, "bad body"); return; }
    const std::string cwd = body.value("cwd", std::string{});
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    const std::string id     = ticket_id_from_path(req);
    const std::string status = body.value("status", std::string{});

    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) {
        tickets_error(res, err == "no board" ? 404 : 500, err);
        return;
    }
    const auto col_keys = column_keys_of(board);
    if (!col_keys.count(status)) {
        tickets_error(res, 400, "invalid status");
        return;
    }
    json * target = nullptr;
    if (board["tickets"].is_array()) {
        for (auto & t : board["tickets"]) {
            if (t.value("id", std::string{}) == id) { target = &t; break; }
        }
    }
    if (!target) { tickets_error(res, 404, "no ticket"); return; }
    (*target)["status"]  = status;
    (*target)["updated"] = current_iso_utc();
    if (!save_board_atomic(tickets_path_for(cwd), board, err)) {
        tickets_error(res, 500, err);
        return;
    }
    res.set_content(target->dump(2), "application/json");
}

// ===================================================================
// Ticket runner: hand each todo ticket's body to the /api/chat pipeline
// in T-number order, moving the ticket todo -> doing -> done/blocked as
// the pipeline finishes. One run per project cwd; start/stop endpoints
// let the browser drive it from the tickets board toolbar.
// ===================================================================

// ChatRun mirrors TicketRun's shape for a free-form chat session so a
// browser tab that reloaded mid-turn can reattach without losing the
// pane state. Keyed by session ID (X-Tool-Session, i.e. the SQLite
// session file the chat is being appended to). The pipeline runs
// inline inside handle_chat's chunked content provider (as before);
// every emit() also publishes to run.ring and the subscriber list so
// a later `/api/chat/events` subscriber replays what already happened
// and receives future frames live.
struct ChatRun {
    std::string             sid;
    std::atomic<bool>       running{false};
    struct Subscriber {
        std::function<bool(const std::string &)> write;
        std::atomic<bool> alive{true};
    };
    std::mutex              subs_mu;
    std::vector<std::shared_ptr<Subscriber>> subs;
    struct RingItem { uint64_t seq; std::string frame; };
    std::deque<RingItem>    ring;
    uint64_t                next_seq{1};
    // 400 events / turn is roomy: a very verbose turn hits ~30 layer
    // frames + 200 heartbeats before hitting a 2-3 min ceiling.
    static constexpr size_t kRingMax = 400;
};

static std::mutex                                              g_chat_runs_mu;
static std::unordered_map<std::string, std::shared_ptr<ChatRun>> g_chat_runs;

// Publish one already-formed SSE frame body ("event: X\ndata: Y\n\n")
// to the run's ring + every live subscriber. Prepends the "id: <sess>-<seq>\n"
// header used by the reconnect cursor. Returns the fully tagged frame so
// the direct POST sink can also write it and hand the same seq id back
// to the client. Caller must NOT already hold subs_mu.
static std::string chat_run_publish(ChatRun & run,
                                    const std::string & frame_body);

struct TicketRun {
    std::string             cwd;
    std::atomic<bool>       running{false};
    std::atomic<bool>       stop_requested{false};
    std::mutex              mu;
    std::string             current_ticket_id;
    std::string             last_layer;         // live pipeline layer name
    std::string             last_error;
    std::thread             worker;
    struct Subscriber {
        std::function<bool(const std::string &)> write;
        std::atomic<bool> alive{true};
    };
    std::mutex              subs_mu;
    std::vector<std::shared_ptr<Subscriber>> subs;
    // Replay ring: every broadcast frame is stashed here with a monotonic
    // seq so a reconnecting subscriber can pick up where it left off.
    struct RingItem { uint64_t seq; std::string frame; };
    std::deque<RingItem>    ring;
    uint64_t                next_seq{1};
    static constexpr size_t kRingMax = 200;
};

// Server-lifetime session id. Any client whose stored session differs is
// told to wipe and resubscribe (server restarted, ring is meaningless).
static const uint64_t g_session_id =
    static_cast<uint64_t>(std::chrono::system_clock::now()
                          .time_since_epoch().count());

static std::mutex                                                  g_ticket_runs_mu;
static std::unordered_map<std::string, std::shared_ptr<TicketRun>> g_ticket_runs;
static std::atomic<int>                                            g_local_port{8080};

// Publish a chat SSE frame through the ChatRun subscriber/ring
// pipeline. See declaration above for the reason a chat needs one.
static std::string chat_run_publish(ChatRun & run,
                                    const std::string & frame_body) {
    std::lock_guard<std::mutex> lk(run.subs_mu);
    const uint64_t seq = run.next_seq++;
    const std::string tagged = "id: " + std::to_string(g_session_id) +
                               "-" + std::to_string(seq) + "\n" + frame_body;
    run.ring.push_back({seq, tagged});
    while (run.ring.size() > ChatRun::kRingMax) run.ring.pop_front();
    for (auto & s : run.subs) {
        if (s->alive.load() && !s->write(tagged)) s->alive.store(false);
    }
    return tagged;
}

// Broadcast a lifecycle frame to every live subscriber AND append it to
// the ring so late-joining or reconnecting subscribers can replay it.
// Caller must NOT already hold subs_mu.
static void ticket_run_broadcast_lifecycle(TicketRun & run,
                                           const std::string & frame) {
    std::lock_guard<std::mutex> lk(run.subs_mu);
    const uint64_t seq = run.next_seq++;
    const std::string tagged = "id: " + std::to_string(g_session_id) +
                               "-" + std::to_string(seq) + "\n" + frame;
    run.ring.push_back({seq, tagged});
    while (run.ring.size() > TicketRun::kRingMax) run.ring.pop_front();
    for (auto & s : run.subs) {
        if (s->alive.load() && !s->write(tagged)) s->alive.store(false);
    }
}

// Find the next todo ticket sorted by numeric T-id ascending. Returns
// false when there are no more todos left (or the board is unreadable).
// Return the id of the ticket most recently transitioned to "blocked",
// or "" if no blocked tickets exist. Ties broken by highest T-N.
// Caller must hold g_tickets_mtx.
static std::string ticket_run_most_recently_blocked_locked(const json & board) {
    std::string best_id, best_ts;
    long        best_n = -1;
    for (const auto & t : board["tickets"]) {
        if (t.value("status", std::string{}) != "blocked") continue;
        const std::string id  = t.value("id", std::string{});
        const std::string upd = t.value("updated", std::string{});
        if (id.size() < 3 || id[0] != 'T' || id[1] != '-') continue;
        long n = 0;
        try { n = std::stol(id.substr(2)); } catch (...) { continue; }
        if (upd > best_ts || (upd == best_ts && n > best_n)) {
            best_ts = upd; best_n = n; best_id = id;
        }
    }
    return best_id;
}

static bool ticket_run_next_todo(const std::string & cwd,
                                 std::string & id_out,
                                 std::string & body_out) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board;
    std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) return false;
    if (!board["tickets"].is_array()) return false;
    // If a ticket is currently in "blocked" state, prefer its children
    // (auto-decomposed sub-tickets) so the recovery path runs before
    // the numeric-next todo. Without this bias, sub-tickets minted from
    // board["next_id"] would land at the tail of T-N ordering and the
    // runner would resume older todos instead of the decomposition.
    const std::string blocked_parent =
        ticket_run_most_recently_blocked_locked(board);
    struct Rec { long n; std::string id, body; bool is_child; };
    std::vector<Rec> todos;
    for (const auto & t : board["tickets"]) {
        if (t.value("status", std::string{}) != "todo") continue;
        const std::string id = t.value("id", std::string{});
        if (id.size() < 3 || id[0] != 'T' || id[1] != '-') continue;
        long n = 0;
        try { n = std::stol(id.substr(2)); }
        catch (...) { continue; }
        // .value() throws on non-string types (e.g. explicit null), so
        // walk the value manually and coerce anything non-string to "".
        std::string parent;
        if (t.contains("parent") && t["parent"].is_string()) {
            parent = t["parent"].get<std::string>();
        }
        const bool is_child =
            !blocked_parent.empty() && parent == blocked_parent;
        todos.push_back({ n, id, t.value("body", std::string{}), is_child });
    }
    if (todos.empty()) return false;
    std::sort(todos.begin(), todos.end(),
              [](const Rec & a, const Rec & b) {
                  if (a.is_child != b.is_child) return a.is_child;
                  return a.n < b.n;
              });
    id_out   = todos.front().id;
    body_out = todos.front().body;
    return true;
}

// Insert one or more child tickets under `parent_id`. Each child gets a
// fresh T-N id minted from board["next_id"], status "todo", parent set
// to the parent id, and a label ["decomposed-from-parent"] so it's
// visually distinguishable in the kanban.
static bool ticket_run_insert_subtickets(
    const std::string & cwd,
    const std::string & parent_id,
    const std::vector<std::pair<std::string, std::string>> & children,
    std::vector<std::string> * inserted_out = nullptr) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board;
    std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) return false;
    if (!board["tickets"].is_array()) return false;
    long next_id = board.value("next_id", 1);
    const std::string now = current_iso_utc();
    for (const auto & [title, body] : children) {
        const std::string new_id = "T-" + std::to_string(next_id++);
        json t = json::object();
        t["id"]      = new_id;
        t["title"]   = title;
        t["body"]    = body;
        t["status"]  = "todo";
        t["parent"]  = parent_id;
        t["labels"]  = json::array({"decomposed-from-" + parent_id});
        t["created"] = now;
        t["updated"] = now;
        board["tickets"].push_back(t);
        if (inserted_out) inserted_out->push_back(new_id);
    }
    board["next_id"] = next_id;
    return save_board_atomic(tickets_path_for(cwd), board, err);
}

// Mutate a single ticket's status in-place on disk.
static bool ticket_run_set_status(const std::string & cwd,
                                  const std::string & id,
                                  const std::string & status) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board;
    std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) return false;
    if (!board["tickets"].is_array()) return false;
    for (auto & t : board["tickets"]) {
        if (t.value("id", std::string{}) == id) {
            t["status"]  = status;
            t["updated"] = current_iso_utc();
            return save_board_atomic(tickets_path_for(cwd), board, err);
        }
    }
    return false;
}

// Drive one ticket's body through /api/chat over local HTTP and decide
// success from the `final` event's handler kind. Uses a Request with a
// content_receiver so we can parse SSE frames as they arrive and (a)
// update the runner's `last_layer` for UI progress polling, (b) forward
// each frame to any browser subscribed to /api/tickets/run/events.
// stop_requested causes status::request_cancel to interrupt the
// pipeline at the pulse counter, which closes the stream promptly.
// `sse_out`, when non-null, receives the raw SSE body so the caller can
// persist a per-ticket log for debugging.
static bool ticket_run_execute(int port,
                               const std::string & msg,
                               const std::string & cwd,
                               std::atomic<bool> & stop_requested,
                               std::string * sse_out,
                               TicketRun * run_ptr) {
    httplib::Client cli("127.0.0.1", port);
    cli.set_read_timeout (3600, 0);
    cli.set_write_timeout(3600, 0);
    json req_body{ {"message", msg}, {"cwd", cwd} };

    std::string all_body;   // full response body for the caller
    std::string accum;      // frame parser buffer
    bool saw_final = false;
    bool success   = false;

    auto broadcast = [&](const std::string & frame) {
        if (!run_ptr) return;
        std::lock_guard<std::mutex> lk(run_ptr->subs_mu);
        const uint64_t seq = run_ptr->next_seq++;
        const std::string tagged = "id: " + std::to_string(g_session_id) +
                                   "-" + std::to_string(seq) + "\n" + frame;
        run_ptr->ring.push_back({seq, tagged});
        while (run_ptr->ring.size() > TicketRun::kRingMax)
            run_ptr->ring.pop_front();
        for (auto & s : run_ptr->subs) {
            if (s->alive.load()) {
                if (!s->write(tagged)) s->alive.store(false);
            }
        }
        run_ptr->subs.erase(
            std::remove_if(run_ptr->subs.begin(), run_ptr->subs.end(),
                [](const std::shared_ptr<TicketRun::Subscriber> & s) {
                    return !s->alive.load();
                }),
            run_ptr->subs.end());
    };

    httplib::Request req;
    req.method = "POST";
    req.path   = "/api/chat";
    req.body   = req_body.dump();
    req.headers.emplace("Content-Type", "application/json");
    req.content_receiver =
        [&](const char * data, size_t len,
            uint64_t /*offset*/, uint64_t /*total*/) -> bool {
            if (stop_requested.load()) return false;
            all_body.append(data, len);
            accum.append(data, len);
            std::size_t sep;
            while ((sep = accum.find("\n\n")) != std::string::npos) {
                std::string frame = accum.substr(0, sep);
                accum.erase(0, sep + 2);
                // Forward the raw frame to any browser subscriber.
                broadcast(frame + "\n\n");
                std::string evt, payload;
                std::istringstream ss(frame);
                std::string line;
                while (std::getline(ss, line)) {
                    if      (line.rfind("event: ", 0) == 0) evt      = line.substr(7);
                    else if (line.rfind("data: ",  0) == 0) payload += line.substr(6);
                }
                if (evt == "layer" && !payload.empty() && run_ptr) {
                    json j = json::parse(payload, nullptr, false);
                    if (j.is_object() && j.contains("name")) {
                        std::lock_guard<std::mutex> lk(run_ptr->mu);
                        run_ptr->last_layer = j.value("name", std::string{});
                    }
                }
                if (evt == "final" && !payload.empty()) {
                    json j = json::parse(payload, nullptr, false);
                    if (j.is_object() && j.contains("handler")) {
                        const auto & h = j["handler"];
                        if (h.is_object()) {
                            const std::string kind = h.value("kind", std::string{});
                            if (kind == "shell") {
                                success = h.value("exit_code", 1) == 0;
                            } else if (kind == "answer" || kind == "components_answer" ||
                                       kind == "physics_answer" || kind == "chemistry_answer" ||
                                       kind == "electronics_answer" || kind == "statement" ||
                                       kind == "noted") {
                                success = true;
                            } else if (kind == "image_gen" || kind == "image_edit") {
                                // A generation ticket succeeds when the
                                // handler actually produced a file. If
                                // Chroma failed there's no file_path and
                                // this ticket rightly blocks.
                                success = h.contains("file_path") &&
                                          !h.value("file_path", std::string{}).empty();
                            } else if (kind == "ticket_op") {
                                // Chat-side CRUD from within a ticket
                                // body (e.g. a plan ticket). Successful
                                // when we managed to touch the board.
                                success = h.contains("answer") &&
                                          h.value("answer", std::string{}).find("Failed") == std::string::npos &&
                                          h.value("answer", std::string{}).find("Could not") == std::string::npos;
                            } else {
                                success = false;
                            }
                            saw_final = true;
                        }
                    }
                }
            }
            return true;
        };

    auto res = cli.send(req);
    if (!res) return false;
    if (stop_requested.load()) return false;

    if (sse_out) *sse_out = all_body;
    return saw_final && success && !stop_requested.load();
}

// Collect every .hpp / .h header already committed to the project cwd
// (excluding embedded-asset headers -- those are huge byte arrays and
// carry no useful signature information). Returns a single string
// suitable to prepend to a ticket body so the coder grounds new code
// on real function signatures instead of hallucinating a framework
// (the routes.cpp / http_request / router.add_route class of bug).
static std::string ticket_run_headers_context(const std::string & cwd) {
    namespace fs = std::filesystem;
    std::error_code ec;
    if (!fs::is_directory(cwd, ec)) return {};
    std::vector<fs::path> headers;
    for (fs::recursive_directory_iterator it(cwd, ec), end; it != end;
         it.increment(ec)) {
        if (ec) { ec.clear(); continue; }
        const auto & p = it->path();
        // Skip dotfiles/dot-dirs (.ac9_runs, .tickets.agile, etc.) and
        // any CMake / build dirs so we do not include the CMake compiler
        // detection scratch files.
        {
            const std::string s = p.string();
            if (s.find("/.") != std::string::npos ||
                s.find("/CMakeFiles/") != std::string::npos ||
                s.find("/build/")     != std::string::npos)
                continue;
        }
        if (!it->is_regular_file(ec)) continue;
        const std::string ext = p.extension().string();
        if (ext != ".hpp" && ext != ".h" && ext != ".hxx") continue;
        headers.push_back(p);
    }
    std::sort(headers.begin(), headers.end());
    std::string ctx;
    for (const auto & h : headers) {
        std::error_code sec;
        auto sz = fs::file_size(h, sec);
        if (sec) continue;
        // Skip huge headers -- they are xxd byte-array embeds, not APIs.
        if (sz > 16 * 1024) continue;
        std::ifstream f(h, std::ios::binary);
        if (!f) continue;
        std::ostringstream ss; ss << f.rdbuf();
        std::string body = ss.str();
        // Extra skip: if the content is >70% comma-separated hex bytes,
        // it is an embed even if it is small.
        {
            std::size_t hex_bytes = 0, total = 0;
            for (char c : body) {
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
                    c == ',') continue;
                ++total;
                if (c == '0' && total > 0) ++hex_bytes;
                else if (std::isxdigit(static_cast<unsigned char>(c)))
                    ++hex_bytes;
            }
            if (total > 200 && hex_bytes * 100 >= total * 70) continue;
        }
        // Path relative to cwd, for readable citation.
        std::string rel = fs::relative(h, cwd, ec).string();
        if (ec) { rel = h.string(); ec.clear(); }
        ctx.append("--- ").append(rel).append(" ---\n");
        ctx.append(body);
        if (ctx.empty() || ctx.back() != '\n') ctx.push_back('\n');
        ctx.push_back('\n');
    }
    if (ctx.empty()) return {};
    std::string prefix =
        "EXISTING PROJECT HEADERS (use these EXACT function signatures; "
        "do NOT invent new API shapes, do NOT rename these functions, "
        "do NOT wrap them in a fake http_request / http_response / "
        "router.add_route abstraction that does not exist here):\n\n";
    return prefix + ctx + "\n(End of existing headers.)\n\n";
}

// Layer-0 checkpoint: after every successful ticket, snapshot the
// entire target project directory into <cwd>/.backup/ so a subsequent
// blocked ticket can be undone by copying back. The snapshot is
// atomic-ish (write into .backup.tmp, then rename over .backup) so a
// crash mid-snapshot doesn't leave a half-populated backup.
//
// Skips: .backup itself (we're writing INTO it), .ac9_runs (per-ticket
// SSE logs, huge and useless for restore), .tickets.agile and the
// backup that already lives beside it (persistent state, must survive
// restore), .toolai.cfg (user's per-project config), CMakeFiles/,
// CMakeCache.txt / cmake_install.cmake / Makefile / build/ (build
// artifacts; restoring them is worse than rebuilding).
static bool project_skip_dirent(const std::string & name) {
    if (name == ".backup" || name == ".backup.tmp") return true;
    if (name == ".ac9_runs") return true;
    if (name == ".tickets.agile") return true;
    if (name.rfind(".tickets.agile.bak.", 0) == 0) return true;
    if (name == ".toolai.cfg") return true;
    if (name == ".ac9ai.cfg") return true;
    if (name == "CMakeFiles" || name == "CMakeCache.txt") return true;
    if (name == "cmake_install.cmake" || name == "Makefile") return true;
    if (name == "build") return true;
    return false;
}

static void project_copy_tree(const fs::path & src, const fs::path & dst) {
    std::error_code ec;
    fs::create_directories(dst, ec);
    for (fs::directory_iterator it(src, ec), end; !ec && it != end; it.increment(ec)) {
        const auto & p = it->path();
        if (project_skip_dirent(p.filename().string())) continue;
        const auto target = dst / p.filename();
        if (fs::is_directory(p, ec)) {
            project_copy_tree(p, target);
        } else if (fs::is_regular_file(p, ec) || fs::is_symlink(p, ec)) {
            fs::copy_file(p, target,
                fs::copy_options::overwrite_existing, ec);
        }
    }
}

static void project_snapshot(const std::string & cwd) {
    const fs::path root = cwd;
    const fs::path tmp  = root / ".backup.tmp";
    const fs::path good = root / ".backup";
    std::error_code ec;
    fs::remove_all(tmp, ec);
    project_copy_tree(root, tmp);
    fs::remove_all(good, ec);
    fs::rename(tmp, good, ec);
    if (ec) {
        std::fprintf(stderr,
            "project_snapshot: rename %s -> %s failed: %s\n",
            tmp.c_str(), good.c_str(), ec.message().c_str());
    }
}

// Layer-1 restore: wipe every file/dir under <cwd> that isn't in the
// skip set, then copy .backup/* back on top. Result: the target project
// looks EXACTLY the way it did right after the last successful ticket.
static void project_restore(const std::string & cwd) {
    const fs::path root = cwd;
    const fs::path good = root / ".backup";
    std::error_code ec;
    if (!fs::exists(good, ec)) {
        std::fprintf(stderr,
            "project_restore: no .backup at %s; nothing to do\n",
            good.c_str());
        return;
    }
    // Wipe non-skipped entries.
    for (fs::directory_iterator it(root, ec), end; !ec && it != end; it.increment(ec)) {
        const auto & p = it->path();
        if (project_skip_dirent(p.filename().string())) continue;
        fs::remove_all(p, ec);
    }
    // Copy .backup/* back into root.
    project_copy_tree(good, root);
}

// Layer-2 auto-decompose: after a ticket blocks and the project has
// been restored, ask coder-big (Qwen3-Coder-30B-A3B-Instruct) to split
// the failing ticket into 2-5 smaller sub-tickets and insert them
// under status "todo" with parent = the failed id.
//
// WHY the coder and not the thinking planner: the decompose task is
// structural transformation ("emit exactly this JSON schema"), not
// reasoning. The thinking planner burns 1500-2500 tokens on <think>
// scaffolding before it emits the answer, which starves the actual
// output on a 2400-token budget. Coder-big is instruct-tuned, has
// zero <think> overhead, was drilled on structured emission (JSON,
// signatures, code blocks), and is warm-cached because it just
// finished the failing fix_build loop -- zero eviction cost.
// Depth cap for Layer-2 auto-decompose. Ticket labels carry a
// "decomposed-from-T-N" tag on every child; if the ticket that just
// blocked already has one, it is itself a sub-ticket and further
// decomposition risks a cascade (observed on the maze-game run:
// 15 planned tickets grew to 96 across 5 nested levels because every
// sub-ticket blocked and split into 2-5 more). Return true when this
// ticket has already been decomposed at least once.
static bool ticket_already_decomposed(const std::string & cwd,
                                       const std::string & id) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    if (!load_board(tickets_path_for(cwd), board, err)) return false;
    if (!board["tickets"].is_array()) return false;
    for (const auto & t : board["tickets"]) {
        if (t.value("id", std::string{}) != id) continue;
        if (!t.contains("labels") || !t["labels"].is_array()) return false;
        for (const auto & l : t["labels"]) {
            if (!l.is_string()) continue;
            if (l.get<std::string>().rfind("decomposed-from-", 0) == 0) return true;
        }
    }
    return false;
}

static bool ticket_run_try_decompose(TicketRun & run,
                                     const std::string & parent_id,
                                     const std::string & parent_body) {
    // Cap: refuse to decompose a ticket that's already a sub-ticket.
    // Prevents the runaway cascade that grew 15 planned tickets into
    // 96 nonsense sub-tickets on the maze-game project.
    if (ticket_already_decomposed(run.cwd, parent_id)) {
        std::fprintf(stderr,
            "decompose: %s is already a sub-ticket; refusing further "
            "decomposition (Layer-2 depth cap 1)\n",
            parent_id.c_str());
        json j{{"parent", parent_id}, {"reason", "already-decomposed-once"}};
        ticket_run_broadcast_lifecycle(run,
            "event: decompose_capped\ndata: " + j.dump() + "\n\n");
        return false;
    }
    static std::once_flag coder_once;
    std::call_once(coder_once, []() {
        try { coder::init(); } catch (...) {}
    });
    static const char * kDecomposeSys =
        "You are a ticket decomposer for an autonomous coding pipeline. "
        "The parent ticket below just blocked because the coder could "
        "not hold coherence across the whole scope in one pass. Break "
        "it into 2 to 5 SMALLER sub-tickets, each producing exactly "
        "ONE focused artifact (one header file, one implementation "
        "file, one build check, one smoke-test invocation) so the "
        "coder can knock them out one at a time. Preserve every "
        "requirement of the parent ticket across the sub-tickets; do "
        "not drop anything. Sub-tickets execute in the order given.\n"
        "\n"
        "OUTPUT REQUIREMENTS:\n"
        "  - Emit exactly one JSON array. No prose before, no prose "
        "after. No markdown code fences (no triple backticks).\n"
        "  - Shape:\n"
        "      [\n"
        "        {\"title\": \"short title\", \"body\": \"self-contained "
        "ticket body\"},\n"
        "        ...\n"
        "      ]\n"
        "  - Every element MUST have both fields. Body should read like "
        "the parent's own body, not like a reference to it.\n"
        "  - Do not mention 'parent ticket', 'previous ticket', or ticket "
        "numbers. Each sub-ticket is self-contained.\n"
        "  - Do not invent APIs. Preserve exact function signatures, "
        "file paths, and byte-size expectations from the parent body.";
    std::string prompt = "PARENT TICKET BODY:\n\n";
    prompt.append(parent_body);
    prompt.append("\n\nOUTPUT the JSON array now.");
    std::fprintf(stderr,
        "decompose: invoking coder for %s (parent body %zu bytes)\n",
        parent_id.c_str(), parent_body.size());
    std::string plan;
    bool truncated = false;
    try {
        plan = coder::generate(kDecomposeSys, prompt,
                               /*max_new_tokens=*/4096, &truncated);
    } catch (const std::exception & ex) {
        std::fprintf(stderr,
            "decompose: coder threw for %s: %s\n",
            parent_id.c_str(), ex.what());
        return false;
    }
    std::fprintf(stderr,
        "decompose: raw plan output (%zu chars, truncated=%d) for %s:\n"
        "--- BEGIN PLAN ---\n%s\n--- END PLAN ---\n",
        plan.size(), (int) truncated, parent_id.c_str(), plan.c_str());
    // Locate the first '[' and the last ']' so we tolerate a stray line
    // of prose the model may still emit despite the strict prompt.
    const std::size_t lb = plan.find('[');
    const std::size_t rb = plan.rfind(']');
    if (lb == std::string::npos || rb == std::string::npos || rb <= lb) {
        std::fprintf(stderr,
            "decompose: could not locate JSON array in coder output "
            "for %s\n", parent_id.c_str());
        return false;
    }
    json arr = json::parse(plan.substr(lb, rb - lb + 1), nullptr, false);
    if (!arr.is_array() || arr.empty()) {
        std::fprintf(stderr,
            "decompose: coder output was not a non-empty JSON array "
            "for %s (parse produced %s)\n",
            parent_id.c_str(),
            arr.is_discarded() ? "parse-error" : "wrong-type");
        return false;
    }
    std::vector<std::pair<std::string, std::string>> children;
    for (const auto & c : arr) {
        if (!c.is_object()) continue;
        std::string title = c.value("title", std::string{});
        std::string body  = c.value("body",  std::string{});
        if (title.empty() || body.empty()) continue;
        children.emplace_back(std::move(title), std::move(body));
    }
    if (children.empty()) {
        std::fprintf(stderr,
            "decompose: no valid sub-tickets in coder output for %s "
            "(array had %zu entries, none with both title+body)\n",
            parent_id.c_str(), arr.size());
        return false;
    }
    std::vector<std::string> inserted;
    if (!ticket_run_insert_subtickets(run.cwd, parent_id, children,
                                      &inserted)) {
        std::fprintf(stderr,
            "decompose: insert_subtickets failed for parent %s\n",
            parent_id.c_str());
        return false;
    }
    {
        json j{ {"parent", parent_id}, {"children", inserted} };
        ticket_run_broadcast_lifecycle(
            run,
            "event: decompose\ndata: " + j.dump() + "\n\n");
    }
    std::fprintf(stderr,
        "decompose: %s split into %zu sub-tickets, first: %s\n",
        parent_id.c_str(), inserted.size(),
        inserted.empty() ? "(none)" : inserted.front().c_str());
    return true;
}

static void ticket_run_worker(std::shared_ptr<TicketRun> run) {
    const int port = g_local_port.load();
    while (!run->stop_requested.load()) {
        std::string id, body;
        if (!ticket_run_next_todo(run->cwd, id, body)) break;
        {
            std::lock_guard<std::mutex> lk(run->mu);
            run->current_ticket_id = id;
        }
        ticket_run_set_status(run->cwd, id, "doing");
        // Start benchmark accumulation for this ticket. Every model call
        // between here and bench::end_ticket() gets recorded into
        // <cwd>/.ac9_runs/<id>.bench.jsonl for hardware-sizing analysis.
        bench::begin_ticket(id, run->cwd);
        // Announce the new ticket so any browser subscriber can open a
        // fresh chat message. Then run the pipeline.
        {
            json j{ {"id", id}, {"body", body} };
            ticket_run_broadcast_lifecycle(
                *run,
                "event: ticket_start\ndata: " + j.dump() + "\n\n");
        }
        // Prepend every already-generated project header so the coder
        // grounds new code on real signatures. Without this, routes.cpp
        // hallucinates a Flask/Express-shaped API and later tickets
        // fail with "unknown identifier".
        std::string enriched_body = ticket_run_headers_context(run->cwd) + body;
        std::string sse_body;
        const bool ok = ticket_run_execute(port, enriched_body, run->cwd,
                                           run->stop_requested,
                                           &sse_body,
                                           run.get());
        // Persist the raw SSE for postmortem debugging under
        // <cwd>/.ac9_runs/<id>.log. Best-effort; failure is silent.
        try {
            std::filesystem::path dir = std::filesystem::path(run->cwd)
                                        / ".ac9_runs";
            std::filesystem::create_directories(dir);
            std::ofstream f(dir / (id + ".log"), std::ios::binary | std::ios::trunc);
            if (f) {
                f << "# ticket " << id << " ok=" << (ok ? "true" : "false")
                  << " ts=" << current_iso_utc() << "\n\n";
                f.write(sse_body.data(),
                        static_cast<std::streamsize>(sse_body.size()));
            }
        } catch (...) {}
        if (run->stop_requested.load()) {
            // Put the ticket back in todo so it's the first thing picked
            // up on the next run. Chat may still be draining but we own
            // the ticket now.
            ticket_run_set_status(run->cwd, id, "todo");
            bench::end_ticket();
            break;
        }
        ticket_run_set_status(run->cwd, id, ok ? "done" : "blocked");
        {
            json j{ {"id", id}, {"ok", ok} };
            ticket_run_broadcast_lifecycle(
                *run,
                "event: ticket_end\ndata: " + j.dump() + "\n\n");
        }
        // Flush the accumulated benchmark records to
        // <cwd>/.ac9_runs/<id>.bench.jsonl and stderr the summary.
        bench::end_ticket();
        if (ok) {
            // Layer-0: refresh the last-known-good snapshot so a future
            // blocked ticket can be undone by copying back.
            try { project_snapshot(run->cwd); } catch (const std::exception & ex) {
                std::fprintf(stderr,
                    "ticket_run_worker: snapshot after %s failed: %s\n",
                    id.c_str(), ex.what());
            }
        } else {
            // Layer-1: restore the target project to the last-known-good
            // state before any recovery path runs. That undoes whatever
            // the failing ticket wrote so decompose/repair sees the
            // pristine surface the ticket was supposed to build on.
            try { project_restore(run->cwd); } catch (const std::exception & ex) {
                std::fprintf(stderr,
                    "ticket_run_worker: restore after %s failed: %s\n",
                    id.c_str(), ex.what());
            }
            // Layer-2: ask the thinking planner to decompose the failing
            // ticket into 2-5 smaller sub-tickets. On success, don't
            // break; the outer loop will pick up the first sub-ticket
            // on its next iteration (ticket_run_next_todo now prefers
            // children of the most-recently-blocked ticket).
            if (ticket_run_try_decompose(*run, id, body)) {
                std::fprintf(stderr,
                    "ticket_run_worker: %s decomposed; continuing "
                    "with sub-tickets\n", id.c_str());
                continue;
            }
            // Decompose failed; halt the run and wait for the operator.
            // (Layer-3 self-repair + Layer-4 operator prompt hook in
            // between here in future work.)
            json rb{ {"id", id}, {"reason", "ticket_blocked"} };
            ticket_run_broadcast_lifecycle(
                *run,
                "event: run_paused\ndata: " + rb.dump() + "\n\n");
            std::fprintf(stderr,
                "ticket_run_worker: %s blocked; project restored, "
                "decompose failed; halting run\n", id.c_str());
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lk(run->mu);
        run->current_ticket_id.clear();
    }
    run->stop_requested.store(false);
    run->running.store(false);
}

// POST /api/tickets/run/start  {cwd}
void handle_tickets_run_start(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("cwd")) {
        tickets_error(res, 400, "missing cwd");
        return;
    }
    const std::string cwd = body["cwd"].get<std::string>();
    std::shared_ptr<TicketRun> run;
    {
        std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
        auto it = g_ticket_runs.find(cwd);
        if (it != g_ticket_runs.end() && it->second->running.load()) {
            tickets_error(res, 409, "already running");
            return;
        }
        if (it != g_ticket_runs.end()) {
            // Reuse the existing TicketRun. Any SSE subscribers that
            // latched onto it via handle_tickets_run_events (e.g. after
            // the server restarted and the client's reader loop
            // reconnected before the human clicked Start) survive the
            // reuse and receive the run_start + ticket_start / layer
            // frames we're about to broadcast. If we replaced the entry
            // here with a fresh make_shared<TicketRun>, those pre-attached
            // subscribers would end up pointing at the orphaned old run
            // and see nothing until the next full page refresh.
            run = it->second;
            if (run->worker.joinable()) run->worker.detach();
            run->stop_requested.store(false);
            run->running.store(true);
        } else {
            run = std::make_shared<TicketRun>();
            run->cwd = cwd;
            run->running.store(true);
            g_ticket_runs[cwd] = run;
        }
    }
    // Broadcast run_start so any browser subscriber can wipe its chat
    // display before frames from the first ticket arrive. Also clear the
    // ring: prior tickets are irrelevant to the new run and replaying
    // them would double-render on a client reconnect.
    {
        std::lock_guard<std::mutex> lk(run->subs_mu);
        run->ring.clear();
    }
    ticket_run_broadcast_lifecycle(*run, "event: run_start\ndata: {}\n\n");
    run->worker = std::thread(ticket_run_worker, run);
    run->worker.detach();
    res.set_content(R"({"ok":true,"running":true})", "application/json");
}

// POST /api/tickets/repair  {cwd, ticket_id, correction_message}
// Layer-4 operator escape hatch. Prepends a "CORRECTION FROM OPERATOR:"
// preamble to the named ticket's body and flips its status back to
// "todo" so the runner picks it up on the next Start. Intended for use
// by Claude (or a human) when the automated recovery layers gave up on
// a ticket and it's sitting in blocked. Claude does NOT edit the work
// product; she posts a correction message here and lets ac9's coder
// re-attempt the same ticket with the extra guidance.
void handle_tickets_repair(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("cwd") ||
        !body.contains("ticket_id") || !body.contains("correction_message")) {
        tickets_error(res, 400,
            "missing cwd / ticket_id / correction_message");
        return;
    }
    const std::string cwd  = body["cwd"].get<std::string>();
    const std::string tid  = body["ticket_id"].get<std::string>();
    const std::string note = body["correction_message"].get<std::string>();
    // Reject if there's an in-flight run for this cwd; the operator must
    // stop first so we're not racing the worker on the same ticket.
    {
        std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
        auto it = g_ticket_runs.find(cwd);
        if (it != g_ticket_runs.end() && it->second->running.load()) {
            tickets_error(res, 409,
                "run in progress; stop it first, then repair, then start");
            return;
        }
    }
    // Mutate the ticket in-place: prepend the correction, flip status.
    {
        std::lock_guard<std::mutex> lk(g_tickets_mtx);
        json board;
        std::string err;
        if (!load_board(tickets_path_for(cwd), board, err)) {
            tickets_error(res, 500, err); return;
        }
        if (!board["tickets"].is_array()) {
            tickets_error(res, 500, "board.tickets missing"); return;
        }
        bool found = false;
        for (auto & t : board["tickets"]) {
            if (t.value("id", std::string{}) != tid) continue;
            const std::string orig = t.value("body", std::string{});
            std::string preamble;
            preamble.append("CORRECTION FROM OPERATOR (previous attempt "
                            "was blocked; apply this guidance):\n\n")
                    .append(note)
                    .append("\n\n---\n\nORIGINAL TICKET BODY:\n\n");
            t["body"]    = preamble + orig;
            t["status"]  = "todo";
            t["updated"] = current_iso_utc();
            // Push a label so the operator can see this was repaired.
            if (!t.contains("labels") || !t["labels"].is_array()) {
                t["labels"] = json::array();
            }
            t["labels"].push_back("repair-prompted");
            found = true;
            break;
        }
        if (!found) {
            tickets_error(res, 404,
                "no ticket with id \"" + tid + "\" in this project");
            return;
        }
        if (!save_board_atomic(tickets_path_for(cwd), board, err)) {
            tickets_error(res, 500, err); return;
        }
    }
    json out{ {"ok", true}, {"ticket_id", tid},
              {"status", "todo"},
              {"note", "correction prepended; POST /api/tickets/run/start "
                       "to resume."} };
    res.set_content(out.dump(), "application/json");
}

// POST /api/tickets/run/stop  {cwd}
void handle_tickets_run_stop(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("cwd")) {
        tickets_error(res, 400, "missing cwd");
        return;
    }
    const std::string cwd = body["cwd"].get<std::string>();
    bool was_running = false;
    {
        std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
        auto it = g_ticket_runs.find(cwd);
        if (it != g_ticket_runs.end() && it->second->running.load()) {
            it->second->stop_requested.store(true);
            was_running = true;
        }
    }
    if (was_running) status::request_cancel();
    res.set_content(json{{"ok", true}, {"was_running", was_running}}.dump(),
                    "application/json");
}

// GET /api/tickets/run/status?cwd=X
void handle_tickets_run_status(const httplib::Request & req, httplib::Response & res) {
    const std::string cwd = req.get_param_value("cwd");
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    json out;
    std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
    auto it = g_ticket_runs.find(cwd);
    if (it == g_ticket_runs.end()) {
        out["running"] = false;
        out["current_ticket_id"] = "";
        out["last_layer"]        = "";
    } else {
        out["running"] = it->second->running.load();
        std::lock_guard<std::mutex> lk2(it->second->mu);
        out["current_ticket_id"] = it->second->current_ticket_id;
        out["last_layer"]        = it->second->last_layer;
    }
    res.set_content(out.dump(), "application/json");
}

// GET /api/tickets/run/events?cwd=X
// SSE stream that mirrors the internal /api/chat pipeline output plus
// lifecycle events (ticket_start, ticket_end) so a browser tab can
// render the CLI-driven run's activity live in the chat pane.
void handle_tickets_run_events(const httplib::Request & req, httplib::Response & res) {
    const std::string cwd = req.get_param_value("cwd");
    if (cwd.empty()) { tickets_error(res, 400, "missing cwd"); return; }
    std::shared_ptr<TicketRun> tab;
    {
        std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
        auto it = g_ticket_runs.find(cwd);
        if (it == g_ticket_runs.end()) {
            tab = std::make_shared<TicketRun>();
            tab->cwd = cwd;
            g_ticket_runs[cwd] = tab;
        } else {
            tab = it->second;
        }
    }
    // Parse Last-Event-ID (EventSource-style, header) OR ?since= query
    // param (fetch/reader style: browsers can't set Last-Event-ID on
    // fetch, and this app uses fetch+ReadableStream, not EventSource).
    uint64_t client_session = 0, client_seq = 0;
    auto parse_cursor = [&](const std::string & s) {
        auto dash = s.find('-');
        if (dash == std::string::npos) return;
        try {
            client_session = std::stoull(s.substr(0, dash));
            client_seq     = std::stoull(s.substr(dash + 1));
        } catch (...) { client_session = 0; client_seq = 0; }
    };
    if (req.has_header("Last-Event-ID"))
        parse_cursor(req.get_header_value("Last-Event-ID"));
    if (client_session == 0 && req.has_param("since"))
        parse_cursor(req.get_param_value("since"));
    res.set_chunked_content_provider(
        "text/event-stream",
        [tab, client_session, client_seq](std::size_t,
                                          httplib::DataSink & sink) -> bool {
            auto sub = std::make_shared<TicketRun::Subscriber>();
            sub->alive.store(true);
            auto write_mu = std::make_shared<std::mutex>();
            sub->write = [&sink, write_mu](const std::string & frame) -> bool {
                std::lock_guard<std::mutex> lk(*write_mu);
                return sink.write(frame.data(), frame.size());
            };
            // Session header first so the client can detect restart and
            // wipe its UI if its stored session id no longer matches.
            {
                const std::string hello =
                    "event: session\ndata: {\"id\":\"" +
                    std::to_string(g_session_id) + "\"}\n\n";
                std::lock_guard<std::mutex> lk(*write_mu);
                if (!sink.write(hello.data(), hello.size())) return false;
            }
            {
                std::lock_guard<std::mutex> lk(tab->subs_mu);
                // Replay the ring under the same lock that broadcast()
                // holds: newly-arriving frames queue behind us, so
                // ordering is deterministic and there are no duplicates.
                const bool same_session = (client_session == g_session_id);
                for (const auto & item : tab->ring) {
                    if (same_session && item.seq <= client_seq) continue;
                    if (!sub->write(item.frame)) { sub->alive.store(false); break; }
                }
                if (sub->alive.load()) tab->subs.push_back(sub);
            }
            // Idle loop: keep the connection open with a keepalive comment
            // every second while the subscription is alive. The runner's
            // content_receiver writes real frames directly via sub->write.
            while (sub->alive.load()) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::lock_guard<std::mutex> lk(*write_mu);
                if (!sink.write(": ka\n\n", 6)) {
                    sub->alive.store(false);
                    break;
                }
            }
            sink.done();
            return false;
        });
}

}  // anonymous namespace

// POST /api/terminal/complete {cwd, token, first}
// Tab-completion for the web terminal: runs bash `compgen` in `cwd`.
// `first` marks the command-word position (complete commands) vs an
// argument (complete file/dir paths). Returns candidate full-token
// replacements, directories suffixed with '/'.
void handle_terminal_complete(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object()) {
        res.status = 400;
        res.set_content(R"({"error":"bad body"})", "application/json");
        return;
    }
    std::string cwd   = expand_home(body.value("cwd", std::string{}));
    if (cwd.empty()) cwd = std::getenv("HOME") ? std::getenv("HOME") : "/";
    const std::string token = body.value("token", std::string{});
    const bool first        = body.value("first", false);

    // Bash's default readline completion mixes commands and files after the
    // shell prompt. Our terminal used to hard-switch to file-only mode as
    // soon as `first=false`, so `watch nvidia-s<TAB>` returned nothing --
    // nvidia-smi isn't in cwd, but it is a command on PATH. Include command
    // matches for non-first tokens too when the token doesn't look like a
    // path (no slash and no leading dot). First-position tokens still
    // command-only when they don't contain a slash (matches `ls<TAB>` etc.
    // not showing every file in cwd as an executable candidate).
    const bool token_path_like =
        token.find('/') != std::string::npos ||
        (!token.empty() && token[0] == '.');
    const std::string compgen =
        first && !token_path_like
            ? std::string("compgen -A command -- \"$1\"")
        : (!first && !token_path_like)
            ? std::string("{ compgen -A command -- \"$1\"; compgen -A file -- \"$1\"; }")
            : std::string("compgen -A file -- \"$1\"");
    const bool cmd_mode = first && !token_path_like;
    // $0 = cwd, $1 = token (bash expands them; the inner script is
    // single-quoted so /bin/sh passes it through verbatim).
    const std::string inner =
        "cd \"$0\" 2>/dev/null && { " + compgen +
        "; } 2>/dev/null | LC_ALL=C sort -u | head -n 400";
    const std::string full = "bash -c " + term_squote(inner) + " " +
        term_squote(cwd) + " " + term_squote(token) + " 2>/dev/null";

    json cands = json::array();
    if (FILE * pipe = ::popen(full.c_str(), "r")) {
        std::string out;
        std::array<char, 4096> buf;
        while (std::size_t n = std::fread(buf.data(), 1, buf.size(), pipe)) {
            out.append(buf.data(), n);
        }
        ::pclose(pipe);
        std::size_t pos = 0;
        while (pos < out.size()) {
            std::size_t eol = out.find('\n', pos);
            std::string line = out.substr(pos, eol == std::string::npos ? std::string::npos : eol - pos);
            pos = (eol == std::string::npos) ? out.size() : eol + 1;
            if (line.empty()) continue;
            if (!cmd_mode) {
                // Mark directories with a trailing slash.
                fs::path p = (line[0] == '/') ? fs::path(line)
                                              : fs::path(cwd) / line;
                std::error_code ec;
                if (fs::is_directory(p, ec) && line.back() != '/') line += '/';
            }
            cands.push_back(line);
        }
    }
    res.set_content(json{{"candidates", cands}}.dump(), "application/json");
}

// Deterministic image-intent detectors. The small models will happily
// misroute "generate a picture of a fluffy kitty" to the shell coder if
// we let classify+expertise decide, because the classifier sees a
// command with subtype=code and dispatches through shell_tool. These
// regex-flavored keyword checks run BEFORE the act dispatch, short-
// circuit to the image_generator / image_editor stubs, and emit their
// own SSE layers so the AI pane shows the routing decision.
struct ImageIntent {
    bool        gen    = false;   // "generate/draw/paint a picture..."
    bool        edit   = false;   // "make it black", "recolor to blue"
    std::string subject;          // "a fluffy kitty"
    std::string edit_op;          // "black", "blue background", ...
    std::string reason;           // human-readable, goes into the SSE layer
};

static std::string img_intent_lowercase(const std::string & s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) out.push_back(static_cast<char>(
        std::tolower(static_cast<unsigned char>(c))));
    return out;
}

static bool img_intent_contains_any(const std::string & lc,
    std::initializer_list<const char *> needles) {
    for (const char * n : needles) if (lc.find(n) != std::string::npos) return true;
    return false;
}

// Detect "make an image of X" / "draw a picture of X" / "generate a
// photo of X" / "render an illustration of X". Also matches "picture
// of a fluffy kitty" as a plain noun phrase (chat's classifier tends to
// mark that as a command). The subject is what follows "of ".
static ImageIntent detect_image_gen_intent(const std::string & resolved) {
    ImageIntent it;
    const std::string lc = img_intent_lowercase(resolved);
    // Tool-name bail-out. When the prompt references the ac9 image tools
    // by name ("use the image_gen tool", "run image_generator on", "call
    // image editor to..."), it is a caller-INSTRUCTION to invoke the
    // tool, not a chat request FOR an image. The runner sending T-1
    // 'Use the image_gen tool to create six PNG sprite files' used to
    // land in Chroma with the whole ticket body as the subject and burn
    // a full 20-step Chroma render on gibberish. Ignore these prompts
    // here and let the plan / coder / shell path handle them.
    static const std::regex tool_ref_re(
        R"((image[_ ]gen(erator)?( tool)?)|(image[_ ]edit(or)?( tool)?))",
        std::regex::icase);
    if (std::regex_search(resolved, tool_ref_re)) return it;

    const bool has_medium = img_intent_contains_any(lc,
        {"picture", "image", "photo", "photograph", "artwork",
         "illustration", "drawing", "painting", "sketch", "render",
         "wallpaper", "poster", "logo", "portrait", "sprite", "tile",
         "icon"});
    const bool has_generate_verb = img_intent_contains_any(lc,
        {"generate", "make", "create", "produce", "give me",
         "show me", "draw", "paint", "sketch", "render", "output",
         "conjure", "cook up"});
    // "of <subject>" is the strongest signal a subject follows.
    std::size_t of_pos = lc.find(" of ");
    // Very short prompts like "a fluffy kitty" alone (after a prior
    // image_gen turn) will be caught by detect_image_edit_intent below;
    // here we require BOTH a medium noun and a generation verb, so a
    // question like "what is a picture of dorian gray about" doesn't
    // trip the router.
    if (has_medium && has_generate_verb) {
        it.gen    = true;
        it.reason = "image-gen keywords (medium + generation verb)";
        if (of_pos != std::string::npos) {
            it.subject = resolved.substr(of_pos + 4);
        } else {
            it.subject = resolved;
        }
    }
    return it;
}

// Extract an explicit output path from an image-gen request. Recognized
// phrasings (case-insensitive):
//   save to assets/robot.png
//   save as assets/robot.png
//   output to assets/robot.png
//   save it as `assets/robot.png`
//   write to assets/robot.png
// Returns the path exactly as written (relative or absolute). Empty
// string when nothing matches — caller falls back to the standard
// slugified-timestamp name under <cwd>/.ac9_images/.
static std::string extract_image_save_path(const std::string & resolved) {
    static const std::regex save_re(
        R"((?:save|output|write|store)(?:\s+it)?\s+(?:to|as|into|in)\s+["'`]?([A-Za-z0-9_./\-]+\.(?:png|jpg|jpeg|webp|gif|bmp))["'`]?)",
        std::regex::icase);
    std::smatch m;
    if (std::regex_search(resolved, m, save_re)) return m[1].str();
    return {};
}

// After image_generator lands its slugified PNG in <cwd>/.ac9_images/,
// copy it to the caller's requested path (relative to cwd, unless
// absolute). Creates parent dirs. Returns the final path on success.
static std::string image_land_at_hint(const std::string & cwd,
                                       const std::string & source,
                                       const std::string & hint) {
    namespace fs2 = std::filesystem;
    if (hint.empty() || source.empty()) return {};
    fs2::path dst(hint);
    if (dst.is_relative()) {
        if (!cwd.empty()) dst = fs2::path(expand_home(cwd)) / dst;
        else return {};
    }
    std::error_code ec;
    fs2::create_directories(dst.parent_path(), ec);
    fs2::copy_file(source, dst,
                   fs2::copy_options::overwrite_existing, ec);
    if (ec) return {};
    return dst.string();
}

// Detect an image-editing instruction. Two signals:
//   1) "make it/them/the X <adjective/color>" — imperative recolor / restyle.
//   2) "recolor", "change the color", "edit the image", "modify the
//      picture", "turn it <color>", "swap ... for ..." — explicit edits.
static ImageIntent detect_image_edit_intent(const std::string & resolved) {
    ImageIntent it;
    const std::string lc = img_intent_lowercase(resolved);
    const bool imperative_it =
        lc.find("make it ")   != std::string::npos ||
        lc.find("make them ") != std::string::npos ||
        lc.find("make the ")  != std::string::npos ||
        lc.find("turn it ")   != std::string::npos ||
        lc.find("turn the ")  != std::string::npos ||
        lc.find("change it ") != std::string::npos ||
        lc.find("change the color") != std::string::npos ||
        lc.find("recolor")    != std::string::npos ||
        lc.find("repaint")    != std::string::npos ||
        lc.find("colorize")   != std::string::npos ||
        lc.find("colorise")   != std::string::npos ||
        // "modify it / modify them / modify the kitty / modify to be
        // black" — the user's follow-up phrasing when they don't say
        // "make". Gated by session_has_recent_image() in the caller,
        // so this doesn't misfire on "modify the CMakeLists" during a
        // coding session with an old kitty row in memory unless the
        // whole prompt is short and edit-shaped.
        lc.find("modify it")   != std::string::npos ||
        lc.find("modify them") != std::string::npos ||
        lc.find("modify the ") != std::string::npos ||
        lc.find("modify to ")  != std::string::npos ||
        lc.find("edit it")     != std::string::npos ||
        lc.find("update it")   != std::string::npos ||
        lc.find("redo it")     != std::string::npos ||
        lc.find("but make ")   != std::string::npos ||   // "but make it black"
        lc.find("this time")   != std::string::npos;     // "same picture but this time black"
    const bool explicit_edit = img_intent_contains_any(lc,
        {"edit the image", "edit the picture", "edit the photo",
         "modify the image", "modify the picture",
         "erase from the image", "swap ", "replace it with",
         "add to the image", "remove from the image"});
    if (imperative_it || explicit_edit) {
        it.edit    = true;
        it.reason  = imperative_it ? "imperative recolor / restyle keyword"
                                   : "explicit edit-the-image keyword";
        it.edit_op = resolved;
    }
    return it;
}

// True when the session has a very recent image_gen or image_edit turn.
// The stateless coder would have blown the image right off the flow
// without this; the edit detector uses it to disambiguate "make it
// black" (an edit) from a plain conversational fragment.
static bool session_has_recent_image() {
    try {
        for (const auto & r : context::by_layer("image", 4)) {
            if (r.kind == "gen" || r.kind == "edit") return true;
        }
    } catch (...) {}
    return false;
}

// True when the prompt itself carries a strong "which image?" signal
// (either an explicit filename like `foo.png` or a target-noun like
// "the sunset picture" while the project has any images to look at).
// This lets an edit-shaped prompt bypass session_has_recent_image() so
// a fresh session can still say "edit black-kitty.png make it blue" and
// route to the resolver + editor instead of falling through to chat.
static bool prompt_has_image_edit_hint(const std::string & prompt,
                                       const std::string & cwd) {
    static const std::regex file_re(
        R"([A-Za-z0-9_.\-/]+\.(?:png|jpg|jpeg|webp|gif|bmp))",
        std::regex::icase);
    if (std::regex_search(prompt, file_re)) return true;
    const std::string lc = img_intent_lowercase(prompt);
    const bool has_desc_noun =
        lc.find(" image")   != std::string::npos ||
        lc.find(" picture") != std::string::npos ||
        lc.find(" photo")   != std::string::npos ||
        lc.find(" png")     != std::string::npos ||
        lc.find(" jpg")     != std::string::npos ||
        lc.find(" jpeg")    != std::string::npos ||
        lc.find(" webp")    != std::string::npos;
    if (!has_desc_noun) return false;
    try { return image_resolver::project_has_any_image(cwd); }
    catch (...) { return false; }
}

// The image-edit body. Called from both the early image short-circuit
// (before understanding) and the later act-dispatch backstop. Runs the
// resolver cascade (filename -> session -> vision-description), and
// depending on the outcome either:
//   - dispatches to the raster editor with the resolved input path;
//   - responds with an ambiguity prompt listing candidates for the
//     user to disambiguate;
//   - responds with a not-found message enumerating what was tried.
// Populates `handler` in place and emits SSE `layer` frames for the
// resolver trace + final result. Leaves the synthetic `final` frame
// composition to the caller (both call sites already have their own).
static void run_image_edit_route(
    const std::string & cwd,
    const ImageIntent & edit,
    const std::string & out_dir,
    json & handler,
    const std::function<void(const char *, const json &)> & emit)
{
    image_resolver::Match m;
    try {
        m = image_resolver::resolve(cwd, edit.edit_op);
    } catch (const std::exception & ex) {
        m.kind   = image_resolver::Match::Kind::NotFound;
        m.reason = std::string("resolver threw: ") + ex.what();
    } catch (...) {
        m.kind   = image_resolver::Match::Kind::NotFound;
        m.reason = "resolver threw an unknown exception";
    }

    // Surface the resolver's breadcrumb trace as an image_resolve layer
    // so the AI pane shows the reasoning live (cascade steps + outcome).
    {
        std::string trace = "resolver reason: " + m.reason;
        if (!m.steps.empty()) {
            trace += "\ntrace:";
            for (const auto & s : m.steps) trace += "\n  - " + s;
        }
        emit("layer", {{"name", "image_resolve"}, {"content", trace}});
    }

    handler["kind"]    = "image_edit";
    handler["request"] = edit.edit_op;

    if (m.kind == image_resolver::Match::Kind::Ambiguous) {
        std::ostringstream body;
        body << "I found " << m.candidates.size()
             << " image(s) that could plausibly match — please tell me which one:\n";
        for (const auto & c : m.candidates) {
            char scorebuf[16];
            std::snprintf(scorebuf, sizeof(scorebuf), "%.2f", c.score);
            body << "  * `" << c.basename << "`";
            if (!c.description.empty()) body << " — " << c.description;
            body << "  (" << c.why << ", score " << scorebuf << ")\n";
        }
        body << "\nReply with the exact filename (e.g. `edit "
             << (m.candidates.empty() ? std::string("foo.png")
                                       : m.candidates.front().basename)
             << " ...`).";
        const std::string msg = body.str();
        handler["answer"] = msg;
        json cands = json::array();
        for (const auto & c : m.candidates) {
            cands.push_back({
                {"path",        c.path},
                {"basename",    c.basename},
                {"description", c.description},
                {"score",       c.score},
                {"why",         c.why},
            });
        }
        handler["candidates"] = std::move(cands);
        emit("layer", {{"name", "image_edit"}, {"content", msg}});
        try { context::append("image", "edit_ambiguous", edit.edit_op); }
        catch (...) {}
        return;
    }

    if (m.kind == image_resolver::Match::Kind::NotFound) {
        std::string msg = "Could not resolve which image to edit: " + m.reason + ".";
        if (!m.steps.empty()) {
            msg += "\n\nTried:";
            for (const auto & s : m.steps) msg += "\n  - " + s;
        }
        msg += "\n\nSpecify a filename (e.g. `edit foo.png ...`) or drop an "
               "image somewhere in the project.";
        handler["answer"] = msg;
        emit("layer", {{"name", "image_edit"}, {"content", msg}});
        return;
    }

    // Found — hand the resolved path to the editor.
    const std::string & last_image = m.path;
    emit("layer", {{"name", "image_edit"},
                   {"content", "running Chroma img2img on " + last_image +
                    "\nresolver: " + m.reason +
                    "\nprompt: " + edit.edit_op}});
    auto r = advanced_raster_image_editor_photoshop_gimp_class::edit(
        last_image, edit.edit_op, out_dir);
    std::string msg;
    if (r.ok) {
        msg = "Edited image saved to `" + r.image_path + "`.";
        handler["file_path"] = r.image_path;
        try { context::append("image", "edit_path", r.image_path, edit.edit_op); }
        catch (...) {}
    } else {
        msg = "Edit failed: " + r.message;
        if (!r.log_tail.empty()) msg += "\n\nsd-cli log tail:\n" + r.log_tail;
    }
    handler["answer"] = msg;
    emit("layer", {{"name", "image_edit"}, {"content", msg}});
    try { context::append("image", "edit", edit.edit_op); }
    catch (...) {}
}

// ===================================================================
// Ticket router (chat-side)
// -------------------------------------------------------------------
// Detect "let's plan the project" / "create tickets for X" / "delete
// T-3" / "move T-5 to done" style chat inputs and dispatch straight to
// the .tickets.agile board without burning the understanding stack.
// Mirrors the image intent path in shape and placement.
// ===================================================================
struct TicketIntent {
    enum class Kind { None, Plan, Create, Patch, Move, Remove, Show, List };
    Kind        kind = Kind::None;
    std::string target_id;   // T-N when a single ticket is addressed
    std::string title;       // for Create
    std::string body;        // for Create / Patch (the parse source)
    std::string status;      // for Move / Patch (a column key)
    std::string goal;        // for Plan (the full user goal)
    std::string reason;      // human-readable why-this-kind
};

// Extract "T-N" from a prompt if present.
static std::string extract_ticket_id(const std::string & prompt) {
    static const std::regex id_re(R"(\bT-(\d+)\b)", std::regex::icase);
    std::smatch m;
    if (std::regex_search(prompt, m, id_re)) return "T-" + m[1].str();
    return {};
}

// Map friendly column phrasing ("done", "in progress", "in-progress")
// to the canonical column key. Returns empty when nothing matches.
static std::string extract_status_hint(const std::string & lc) {
    struct Entry { const char * needle; const char * canon; };
    static const Entry table[] = {
        {"in progress", "doing"}, {"in-progress", "doing"},
        {"blocked",     "blocked"}, {"todo",     "todo"},
        {"doing",       "doing"},   {"done",     "done"},
        {"complete",    "done"},    {"completed","done"},
        {"finish",      "done"},    {"finished", "done"},
        {"close",       "done"},    {"closed",   "done"},
    };
    for (const auto & e : table) {
        if (lc.find(e.needle) != std::string::npos) return e.canon;
    }
    return {};
}

// Deterministic ticket-intent detector. Cheap keyword+regex only.
// `cwd` is used for the fallback "board exists → assume plan" branch.
static TicketIntent detect_ticket_intent(const std::string & resolved,
                                         const std::string & cwd) {
    TicketIntent it;
    const std::string lc = img_intent_lowercase(resolved);
    const std::string id = extract_ticket_id(resolved);

    // ---- Plan (bulk) ----
    const bool plan_verb = img_intent_contains_any(lc,
        {"create tickets", "make tickets", "generate tickets",
         "write tickets", "add tickets", "build tickets",
         "make a plan", "create a plan", "build a plan",
         "make the project plan", "make a project plan",
         "create the project plan", "project plan", "sprint plan",
         "make the plan", "create the plan"});
    if (plan_verb) {
        it.kind   = TicketIntent::Kind::Plan;
        it.goal   = resolved;
        it.reason = "plan keywords";
        return it;
    }

    // ---- Single-target ops (T-N mentioned) ----
    if (!id.empty()) {
        // Remove / delete
        if (img_intent_contains_any(lc,
                {"delete ", "remove ", "trash ", "discard ", "drop ticket"})) {
            it.kind = TicketIntent::Kind::Remove;
            it.target_id = id;
            it.reason = "remove verb + " + id;
            return it;
        }
        // Move (verb + column hint OR bare "T-N to done" phrasing)
        const std::string sh = extract_status_hint(lc);
        if (!sh.empty() && img_intent_contains_any(lc,
                {"move ", " to ", "close ", "reopen ", "start work",
                 "block ", "unblock ", "mark "})) {
            it.kind = TicketIntent::Kind::Move;
            it.target_id = id;
            it.status = sh;
            it.reason = "move verb + " + id + " -> " + sh;
            return it;
        }
        // Patch (any change-shaped verb + T-N)
        if (img_intent_contains_any(lc,
                {"change ", "update ", "edit ", "modify ",
                 "set ", "rename ", "make ", "raise ", "lower ", "bump ",
                 "add to ", "append "})) {
            it.kind = TicketIntent::Kind::Patch;
            it.target_id = id;
            it.body      = resolved;
            it.reason    = "patch verb + " + id;
            return it;
        }
        // Show / read
        if (img_intent_contains_any(lc,
                {"show ", "read ", "display ", "print ",
                 "what is ", "what does ", "tell me about ", "describe "})) {
            it.kind = TicketIntent::Kind::Show;
            it.target_id = id;
            it.reason = "show verb + " + id;
            return it;
        }
    }

    // ---- Create single ("add a ticket for X") ----
    if (img_intent_contains_any(lc,
            {"add a ticket", "create a ticket", "new ticket",
             "make a ticket", "add ticket for", "create ticket for",
             "add one ticket", "make one ticket", "another ticket"})) {
        it.kind   = TicketIntent::Kind::Create;
        it.title  = resolved;
        it.reason = "single-create keywords";
        return it;
    }

    // ---- List / show board ----
    if (img_intent_contains_any(lc,
            {"list tickets", "show tickets", "show the tickets",
             "show board", "show the board", "list the tickets",
             "list the board", "what's on the board", "whats on the board",
             "show sprint", "show backlog", "show plan", "show the plan"})) {
        it.kind = TicketIntent::Kind::List;
        it.reason = "list keywords";
        return it;
    }

    // No ticket intent detected.
    (void) cwd;  // reserved for future gate-on-board-existence heuristics
    return it;
}

// Ask the planner (or coder, on planner failure) to decompose `goal`
// into 4–10 short tickets. Returns [(title, body), ...] or empty.
struct PlanTicket { std::string title; std::string body; };

static std::vector<PlanTicket> ticket_plan_from_llm(
    const std::string & goal,
    std::string       * why,
    const std::string & system_override = {})
{
    static std::once_flag planner_once;
    std::call_once(planner_once, []() {
        try { planner::init(); } catch (...) {}
    });
    static std::once_flag coder_once;
    std::call_once(coder_once, []() {
        try { coder::init(); } catch (...) {}
    });
    // Default baseline used when no tool_router shape is supplied. The
    // richer, project-specific shape rules ("one sprite = one ticket",
    // "no tool names in bodies", "cpp-httplib for C++ web servers")
    // live in the tool_router's ticket_plan template and arrive via
    // system_override.
    static const char * kPlanSys =
        "You are a project-plan decomposer. Given a project goal, "
        "produce a set of SHORT, SELF-CONTAINED tickets that a coding "
        "assistant can execute one at a time in the order given. Each "
        "ticket body IS a prompt: written in imperative voice, "
        "unambiguous, actionable. No cross-references to other tickets. "
        "No plan-level commentary. Aim for 4-10 tickets; more if the "
        "goal genuinely needs it. Each title <= 60 characters, each "
        "body <= 400 characters.\n"
        "\n"
        "OUTPUT REQUIREMENTS:\n"
        "  - Emit exactly one JSON array. No prose before or after. No "
        "markdown code fences (no triple backticks).\n"
        "  - Shape: [{\"title\": \"...\", \"body\": \"...\"}, ...]\n"
        "  - Every element MUST have both fields non-empty.";
    const std::string sys =
        system_override.empty() ? std::string(kPlanSys) : system_override;
    std::string raw;
    bool truncated = false;
    // Try planner first (thinking-optimized).
    try {
        raw = planner::generate(sys, goal, 4096, &truncated);
    } catch (const std::exception & ex) {
        std::fprintf(stderr,
            "ticket_plan: planner threw: %s (falling back to coder)\n",
            ex.what());
        raw.clear();
    } catch (...) { raw.clear(); }
    if (raw.empty()) {
        try {
            raw = coder::generate(sys, goal, 4096, &truncated);
        } catch (const std::exception & ex) {
            if (why) *why = std::string("coder threw: ") + ex.what();
            return {};
        }
    }
    const std::size_t lb = raw.find('[');
    const std::size_t rb = raw.rfind(']');
    if (lb == std::string::npos || rb == std::string::npos || rb <= lb) {
        if (why) *why = "no JSON array in LLM output";
        return {};
    }
    json arr = json::parse(raw.substr(lb, rb - lb + 1), nullptr, false);
    if (!arr.is_array() || arr.empty()) {
        if (why) *why = "LLM output was not a non-empty JSON array";
        return {};
    }
    std::vector<PlanTicket> out;
    for (const auto & c : arr) {
        if (!c.is_object()) continue;
        std::string title = c.value("title", std::string{});
        std::string body  = c.value("body",  std::string{});
        if (title.empty() || body.empty()) continue;
        out.push_back({std::move(title), std::move(body)});
    }
    if (out.empty() && why) *why = "array had entries but none with title+body";
    return out;
}

// In-process bulk insert of a plan. Auto-bootstraps the board if
// missing. Returns list of new T-N ids in order.
static std::vector<std::string> ticket_ai_insert_batch(
    const std::string & cwd,
    const std::vector<PlanTicket> & tickets,
    const std::string & label)
{
    std::vector<std::string> ids;
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    const auto path = tickets_path_for(cwd);
    if (!load_board(path, board, err)) board = default_board();
    if (!board["tickets"].is_array()) board["tickets"] = json::array();
    long next_id = board.value("next_id", 1);
    const std::string now = current_iso_utc();
    for (const auto & p : tickets) {
        const std::string new_id = "T-" + std::to_string(next_id++);
        json t = json::object();
        t["id"]       = new_id;
        t["title"]    = p.title;
        t["body"]     = p.body;
        t["status"]   = "todo";
        t["labels"]   = label.empty() ? json::array()
                                       : json::array({label});
        t["priority"] = "normal";
        t["parent"]   = nullptr;
        t["created"]  = now;
        t["updated"]  = now;
        t["extra"]    = json::object();
        board["tickets"].push_back(t);
        ids.push_back(new_id);
    }
    board["next_id"] = next_id;
    if (!save_board_atomic(path, board, err)) return {};
    return ids;
}

// In-process single create. Bootstraps a board if missing.
static bool ticket_ai_create_one(const std::string & cwd,
                                  const std::string & title,
                                  const std::string & body,
                                  std::string & new_id_out,
                                  std::string & err_out) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    const auto path = tickets_path_for(cwd);
    if (!load_board(path, board, err)) board = default_board();
    if (!board["tickets"].is_array()) board["tickets"] = json::array();
    long next_id = board.value("next_id", 1);
    const std::string now = current_iso_utc();
    const std::string id  = "T-" + std::to_string(next_id++);
    json t = json::object();
    t["id"]       = id;
    t["title"]    = title;
    t["body"]     = body;
    t["status"]   = "todo";
    t["labels"]   = json::array();
    t["priority"] = "normal";
    t["parent"]   = nullptr;
    t["created"]  = now;
    t["updated"]  = now;
    t["extra"]    = json::object();
    board["tickets"].push_back(t);
    board["next_id"] = next_id;
    if (!save_board_atomic(path, board, err)) { err_out = err; return false; }
    new_id_out = id;
    return true;
}

// In-process patch: apply the given non-empty fields to `id`.
static bool ticket_ai_patch(const std::string & cwd,
                             const std::string & id,
                             const std::string & new_title,
                             const std::string & new_body,
                             const std::string & new_status,
                             const std::string & new_priority) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    const auto path = tickets_path_for(cwd);
    if (!load_board(path, board, err)) return false;
    const auto col_keys = column_keys_of(board);
    for (auto & t : board["tickets"]) {
        if (t.value("id", std::string{}) != id) continue;
        if (!new_title.empty())    t["title"]    = new_title;
        if (!new_body.empty())     t["body"]     = new_body;
        if (!new_status.empty() && col_keys.count(new_status))
                                    t["status"]   = new_status;
        if (!new_priority.empty() && is_valid_priority(new_priority))
                                    t["priority"] = new_priority;
        t["updated"] = current_iso_utc();
        return save_board_atomic(path, board, err);
    }
    return false;
}

// In-process delete.
static bool ticket_ai_remove(const std::string & cwd, const std::string & id) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    const auto path = tickets_path_for(cwd);
    if (!load_board(path, board, err)) return false;
    if (!board["tickets"].is_array()) return false;
    auto & arr = board["tickets"];
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        if (it->value("id", std::string{}) == id) {
            arr.erase(it);
            return save_board_atomic(path, board, err);
        }
    }
    return false;
}

// In-process status move.
static bool ticket_ai_move(const std::string & cwd,
                            const std::string & id,
                            const std::string & status) {
    std::lock_guard<std::mutex> lk(g_tickets_mtx);
    json board; std::string err;
    const auto path = tickets_path_for(cwd);
    if (!load_board(path, board, err)) return false;
    const auto col_keys = column_keys_of(board);
    if (!col_keys.count(status)) return false;
    for (auto & t : board["tickets"]) {
        if (t.value("id", std::string{}) != id) continue;
        t["status"]  = status;
        t["updated"] = current_iso_utc();
        return save_board_atomic(path, board, err);
    }
    return false;
}

// Shared ticket-router body. Called from the EARLY short-circuit in
// handle_chat. Populates `handler` in place and emits SSE `layer`
// frames for the routing decision + per-operation trace.
static void run_ticket_route(
    const std::string & cwd,
    const TicketIntent & intent,
    json & handler,
    const std::function<void(const char *, const json &)> & emit)
{
    auto kind_name = [](TicketIntent::Kind k) -> const char * {
        using K = TicketIntent::Kind;
        switch (k) {
            case K::None:   return "none";
            case K::Plan:   return "plan";
            case K::Create: return "create";
            case K::Patch:  return "patch";
            case K::Move:   return "move";
            case K::Remove: return "remove";
            case K::Show:   return "show";
            case K::List:   return "list";
        }
        return "?";
    };
    emit("layer", {{"name", "ticket_intent"},
                   {"content", std::string("kind=") + kind_name(intent.kind) +
                    "\nreason: " + intent.reason}});

    handler["kind"] = "ticket_op";

    using K = TicketIntent::Kind;
    switch (intent.kind) {
    case K::Plan: {
        emit("layer", {{"name", "ticket_op"},
                       {"content", "planning tickets for goal:\n" + intent.goal}});
        std::string why;
        const auto plan = ticket_plan_from_llm(intent.goal, &why);
        if (plan.empty()) {
            const std::string msg =
                "Could not build a ticket plan: " +
                (why.empty() ? std::string("LLM returned nothing usable") : why) +
                ".\nTry rephrasing, or use `add a ticket for <X>` to add a single ticket.";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        const auto ids = ticket_ai_insert_batch(cwd, plan, "planned");
        if (ids.empty()) {
            const std::string msg = "Built a plan but failed to persist it "
                "(check .tickets.agile is writable).";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        std::ostringstream body;
        body << "Created " << ids.size() << " tickets ("
             << ids.front() << ".." << ids.back() << "):\n";
        for (std::size_t i = 0; i < plan.size() && i < ids.size(); ++i) {
            body << "  " << ids[i] << " — " << plan[i].title << "\n";
        }
        const std::string msg = body.str();
        handler["answer"]     = msg;
        handler["ticket_ids"] = ids;
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::Create: {
        // Best-effort title extraction: strip "add a ticket for/to " boilerplate.
        std::string title = intent.title;
        const std::string lc = img_intent_lowercase(title);
        std::size_t p = lc.find(" for ");
        if (p == std::string::npos) p = lc.find(" to ");
        if (p != std::string::npos) title = title.substr(p + 5);
        while (!title.empty() && (std::isspace((unsigned char) title.front())
               || title.front() == ',' || title.front() == '.'))
            title.erase(title.begin());
        if (title.empty()) title = intent.title;
        std::string new_id, err;
        if (!ticket_ai_create_one(cwd, title, "", new_id, err)) {
            const std::string msg = "Failed to create ticket: " + err;
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        const std::string msg = "Created " + new_id + " — " + title;
        handler["answer"]     = msg;
        handler["ticket_ids"] = json::array({new_id});
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::Patch: {
        // Minimal field parser: look for common phrasings.
        //   "title to X" / "title is X" / "title = X"
        //   "body to X"
        //   "priority (low|normal|high|urgent)"
        //   status keywords picked up by extract_status_hint.
        const std::string lc = img_intent_lowercase(intent.body);
        std::string new_title, new_body, new_status, new_priority;
        static const std::regex title_re(
            R"(title\s+(?:to|=|is)\s+["']?([^"'\n]+?)["']?\s*(?:$|,|;|\.))",
            std::regex::icase);
        static const std::regex body_re(
            R"(body\s+(?:to|=|is)\s+["']?([^"'\n]+?)["']?\s*(?:$|,|;|\.))",
            std::regex::icase);
        std::smatch m;
        if (std::regex_search(intent.body, m, title_re)) new_title = m[1].str();
        if (std::regex_search(intent.body, m, body_re))  new_body  = m[1].str();
        for (const char * p : {"urgent", "high", "normal", "low"}) {
            if (lc.find(std::string(" ") + p) != std::string::npos) {
                new_priority = p; break;
            }
        }
        new_status = extract_status_hint(lc);
        if (new_title.empty() && new_body.empty() &&
            new_status.empty() && new_priority.empty()) {
            const std::string msg =
                "I need something concrete to patch on " + intent.target_id +
                ": say 'title to X', 'body to Y', 'priority high', or "
                "'move to done'.";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        if (!ticket_ai_patch(cwd, intent.target_id, new_title, new_body,
                              new_status, new_priority)) {
            const std::string msg = "Failed to patch " + intent.target_id +
                " (no such ticket or invalid field).";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        std::ostringstream body;
        body << "Patched " << intent.target_id << ":";
        if (!new_title.empty())    body << " title=" << new_title;
        if (!new_body.empty())     body << " body=<updated>";
        if (!new_status.empty())   body << " status=" << new_status;
        if (!new_priority.empty()) body << " priority=" << new_priority;
        const std::string msg = body.str();
        handler["answer"]     = msg;
        handler["ticket_ids"] = json::array({intent.target_id});
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::Move: {
        if (!ticket_ai_move(cwd, intent.target_id, intent.status)) {
            const std::string msg = "Failed to move " + intent.target_id +
                " to " + intent.status + " (no such ticket or invalid column).";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        const std::string msg = "Moved " + intent.target_id + " to " + intent.status;
        handler["answer"]     = msg;
        handler["ticket_ids"] = json::array({intent.target_id});
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::Remove: {
        if (!ticket_ai_remove(cwd, intent.target_id)) {
            const std::string msg = "Failed to delete " + intent.target_id +
                " (no such ticket).";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        const std::string msg = "Deleted " + intent.target_id;
        handler["answer"]     = msg;
        handler["ticket_ids"] = json::array({intent.target_id});
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::Show: {
        std::lock_guard<std::mutex> lk(g_tickets_mtx);
        json board; std::string err;
        if (!load_board(tickets_path_for(cwd), board, err)) {
            const std::string msg = "No board: " + err;
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        for (const auto & t : board["tickets"]) {
            if (t.value("id", std::string{}) == intent.target_id) {
                std::ostringstream body;
                body << t.value("id", std::string{}) << " — "
                     << t.value("title", std::string{})
                     << "  [" << t.value("status", std::string{}) << "]\n"
                     << t.value("body", std::string{});
                const std::string msg = body.str();
                handler["answer"]     = msg;
                handler["ticket_ids"] = json::array({intent.target_id});
                emit("layer", {{"name", "ticket_op"}, {"content", msg}});
                return;
            }
        }
        const std::string msg = "No ticket " + intent.target_id + " on the board.";
        handler["answer"] = msg;
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::List: {
        std::lock_guard<std::mutex> lk(g_tickets_mtx);
        json board; std::string err;
        if (!load_board(tickets_path_for(cwd), board, err)) {
            const std::string msg = "No board yet.";
            handler["answer"] = msg;
            emit("layer", {{"name", "ticket_op"}, {"content", msg}});
            return;
        }
        std::ostringstream body;
        body << "Board (" << board["tickets"].size() << " tickets):\n";
        std::unordered_map<std::string, std::vector<const json *>> by_status;
        for (const auto & t : board["tickets"]) {
            by_status[t.value("status", std::string("todo"))].push_back(&t);
        }
        for (const auto & col : board["columns"]) {
            const std::string key = col.value("key", std::string{});
            const auto & rows = by_status[key];
            if (rows.empty()) continue;
            body << "\n[" << col.value("title", key) << "]\n";
            for (auto * tp : rows) {
                body << "  " << tp->value("id", std::string{}) << " — "
                     << tp->value("title", std::string{}) << "\n";
            }
        }
        const std::string msg = body.str();
        handler["answer"] = msg;
        emit("layer", {{"name", "ticket_op"}, {"content", msg}});
        return;
    }
    case K::None:
    default:
        break;
    }
}

// POST /api/chat {message: "...", cwd: "..."}
// Streams Server-Sent Events as each pipeline layer completes:
//   event: layer   data: {"name":"cleanup","content":"..."}
//   event: layer   data: {"name":"classify","content":"..."}
//   ...
//   event: final   data: {"act":..., "final":..., "handler":..., "expertise":...}
// Client uses fetch() + reader to parse and render incrementally.
void handle_chat(const httplib::Request & req, httplib::Response & res) {
    json body = json::parse(req.body, nullptr, false);
    if (!body.is_object() || !body.contains("message")) {
        res.status = 400;
        res.set_content(R"({"error":"missing message"})", "application/json");
        return;
    }
    const std::string message = body["message"].get<std::string>();
    const std::string cwd     = body.value("cwd", std::string{});
    const std::string sid     = req.get_header_value("X-Tool-Session");

    // If the client tagged the request with a session id different from the
    // currently-active one, swap before doing any work. Touch last_active.
    if (!sid.empty()) {
        try {
            if (sid != context::current_id() && sessions_store::exists(sid)) {
                context::switch_to(sid);
            }
        } catch (...) { /* fall through; chat still works on current session */ }
        sessions_store::touch(sid);
    }
    if (context::current_id().empty()) {
        res.status = 400;
        res.set_content(R"({"error":"no active session"})", "application/json");
        return;
    }

    // Get-or-create the ChatRun for THIS session so a reloading tab can
    // reattach via /api/chat/events?sid=<sid>. Clear the ring (fresh
    // turn) and mark it running; the pipeline body below flips it back
    // to idle on the way out (RAII scope guard).
    const std::string active_sid = context::current_id();
    std::shared_ptr<ChatRun> run;
    {
        std::lock_guard<std::mutex> lk(g_chat_runs_mu);
        auto it = g_chat_runs.find(active_sid);
        if (it != g_chat_runs.end()) {
            run = it->second;
        } else {
            run = std::make_shared<ChatRun>();
            run->sid = active_sid;
            g_chat_runs[active_sid] = run;
        }
    }
    {
        std::lock_guard<std::mutex> lk(run->subs_mu);
        run->ring.clear();
        run->next_seq = 1;
    }
    run->running.store(true);

    res.set_chunked_content_provider("text/event-stream",
        [message, cwd, run](std::size_t /*offset*/, httplib::DataSink & sink) -> bool {
            // Sink writes come from this thread and the heartbeat thread.
            std::mutex sink_mu;
            // RAII: mark the run idle when the pipeline exits, regardless
            // of how it exits (finished, cancelled, threw). Reload
            // subscribers use this flag to decide whether to synthesize
            // an in-flight bubble on page load.
            struct RunGuard { std::shared_ptr<ChatRun> r;
                              ~RunGuard() { r->running.store(false); } } _rg{run};
            auto emit = [&](const char * evt, const json & data) {
                std::string body = "event: ";
                body.append(evt);
                body.append("\ndata: ");
                body.append(data.dump());
                body.append("\n\n");
                // Publish to the ring + every reload subscriber; the
                // returned string is the frame with the id: header the
                // reconnect cursor uses.
                const std::string tagged = chat_run_publish(*run, body);
                // Also write to THIS response's direct sink (the primary
                // path for the tab that issued the POST).
                std::lock_guard<std::mutex> lk(sink_mu);
                sink.write(tagged.data(), tagged.size());
            };

            // Heartbeat: the model runtimes bump a pulse counter once per
            // decoded token. Emit a heartbeat only when the counter moved,
            // so the client's "thinking" indicator animates only while a
            // model is genuinely producing output and freezes on a stall.
            std::atomic<bool> hb_stop{false};
            std::thread hb([&emit, &hb_stop]() {
                std::uint64_t last = status::pulse_count();
                while (!hb_stop.load(std::memory_order_relaxed)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(700));
                    if (hb_stop.load(std::memory_order_relaxed)) break;
                    const std::uint64_t now = status::pulse_count();
                    // Include the current generate() call's role + token
                    // budget so the client can render "loading (X)" or
                    // "thinking (X)" plus a per-stage progress bar.
                    // Fire on pulse advance OR while a model is loading
                    // (loading = 0 pulses but we still want the UI to
                    // reflect the load).
                    auto ps = status::progress_snapshot();
                    if (now != last || ps.loading) {
                        last = now;
                        json p{{"pulses", now}};
                        if (!ps.role.empty()) {
                            p["role"]    = ps.role;
                            p["loading"] = ps.loading;
                            if (ps.max > 0) {
                                p["tokens"] = ps.current;
                                p["max"]    = ps.max;
                            }
                        }
                        emit("heartbeat", p);
                    }
                }
            });
            struct HbJoin {
                std::atomic<bool> & stop;
                std::thread &       t;
                ~HbJoin() { stop.store(true); if (t.joinable()) t.join(); }
            } hb_join{hb_stop, hb};

            // Cooperative stop (the UI stop button). On cancel: erase the
            // turn from the session store (the prompt goes back to the
            // user's entry field for editing, so it must never replay),
            // tell the client, end the stream.
            const std::uint64_t epoch = status::begin_turn();
            auto bail_if_stopped = [&]() -> bool {
                if (!status::cancelled(epoch)) return false;
                hb_stop.store(true);
                if (hb.joinable()) hb.join();
                context::delete_turn(context::current_turn());
                emit("stopped", json{{"ok", true}});
                sink.done();
                return true;
            };

            try {
                context::next_turn();
                context::append("user", "input", message);

                // The cleanup contract is copy-editing only, never
                // shortening. The small model reliably drops sentences from
                // long inputs, and a long detailed prompt doesn't need typo
                // repair anyway: fidelity beats polish. Skip it entirely for
                // long inputs; for short ones, distrust any output that
                // shrinks noticeably and keep the raw message instead.
                std::string cleaned;
                if (message.size() > 600) {
                    cleaned = message;
                    context::append("cleanup", "output", cleaned, "skipped (long input)");
                    emit("layer", {{"name", "cleanup"},
                                   {"content", "(skipped: long input passed through verbatim)"}});
                } else {
                    cleaned = prompt_cleanup::clean(message);
                    if (cleaned.size() * 100 < message.size() * 85) {
                        cleaned = message;
                        context::append("cleanup", "output", cleaned, "kept raw (cleanup dropped content)");
                        emit("layer", {{"name", "cleanup"},
                                       {"content", "(cleanup dropped content; keeping raw input)\n" + cleaned}});
                    } else {
                        context::append("cleanup", "output", cleaned);
                        emit("layer", {{"name", "cleanup"}, {"content", cleaned}});
                    }
                }

                // Flag set by the LLM tool router below: when true, the
                // legacy EARLY regex routers (ticket / image / parts) are
                // skipped and the pipeline continues straight to
                // classify::analyze + normal act dispatch. The router
                // has spoken; the regex routers must not second-guess
                // it. Observed misfire without this: T-9 body "Create
                // src/index.html ... via new Image() ..." -> tool_router
                // picked coder (correct) at 0.95, coder had no explicit
                // dispatch and fell through, then the legacy image
                // router fired because 'Image()' + 'Create' matched
                // medium+verb and Chroma ran on the HTML ticket body.
                bool router_decided = false;

                // ==== LLM tool router (runs FIRST) ====
                // Ask a small classifier LLM which registered tool best
                // matches the user's intent. When it decides with high
                // confidence, dispatch straight to that tool with the
                // tool-specific system prompt shape (the tool_router's
                // prompt_template — e.g. "one sprite per ticket, no tool
                // names, cpp-httplib" for ticket_plan). This is what
                // "AI-based intent + tool-specific prompt injection"
                // looks like: no more hand-typed shape rules in user
                // prompts, no more per-tool regex detector.
                //
                // Low confidence or "none" -> falls through to the
                // legacy deterministic regex routers below as a safety
                // net for crystal-clear cases (a bare "T-3", a bare
                // "foo.png" filename). The routers themselves stay in
                // place because they are cheap and their misfires are
                // still caught by the "tool = none" no-op return.
                {
                    const auto choice = tool_router::route_and_shape(
                        cleaned, message, /*threshold=*/0.7);
                    if (choice.tool != "none" && choice.confidence >= 0.7) {
                        router_decided = true;   // Suppress legacy routers below.
                        emit("layer", {{"name", "tool_router"},
                                       {"content",
                                        "tool=" + choice.tool +
                                        " confidence=" + std::to_string(choice.confidence) +
                                        " reason=" + choice.reason +
                                        "\nargs=" + choice.args.dump()}});

                        // Router dispatch. Handles the tools whose flow
                        // benefits most from the shape-prompt injection.
                        // Anything else drops through to the legacy
                        // regex routers below.
                        if (choice.tool == "ticket_plan") {
                            const std::string goal =
                                choice.args.contains("goal") &&
                                choice.args["goal"].is_string()
                                    ? choice.args["goal"].get<std::string>()
                                    : message;
                            emit("layer", {{"name", "ticket_op"},
                                           {"content", "planning tickets for goal:\n" + goal}});
                            std::string why;
                            const auto plan = ticket_plan_from_llm(
                                goal, &why, choice.shaped_system);
                            json handler_t;
                            handler_t["kind"]    = "ticket_op";
                            handler_t["request"] = goal;
                            if (plan.empty()) {
                                handler_t["answer"] =
                                    "Could not build a ticket plan: " +
                                    (why.empty() ? std::string("LLM returned nothing usable") : why);
                            } else {
                                const auto ids = ticket_ai_insert_batch(
                                    cwd, plan, "planned");
                                std::ostringstream body;
                                body << "Created " << ids.size() << " tickets ("
                                     << (ids.empty() ? std::string("none")
                                         : ids.front() + ".." + ids.back()) << "):\n";
                                for (std::size_t i = 0;
                                     i < plan.size() && i < ids.size(); ++i) {
                                    body << "  " << ids[i] << " — "
                                         << plan[i].title << "\n";
                                }
                                handler_t["answer"] = body.str();
                                handler_t["ticket_ids"] = ids;
                            }
                            emit("layer", {{"name", "ticket_op"},
                                           {"content", handler_t.value("answer", std::string{})}});
                            classify::Result fake_act;
                            fake_act.act     = "command";
                            fake_act.subtype = "ticket_op";
                            fake_act.tags    = { "tool-router", "ticket_plan" };
                            json fin;
                            fin["act"]       = {{"act", fake_act.act},
                                                {"subtype", fake_act.subtype},
                                                {"tags", fake_act.tags}};
                            fin["final"]     = handler_t.value("answer", std::string{});
                            fin["handler"]   = handler_t;
                            fin["expertise"] = "ticket ops";
                            emit("final", fin);
                            hb_stop.store(true);
                            if (hb.joinable()) hb.join();
                            sink.done();
                            return false;
                        }

                        if (choice.tool == "ticket_list" ||
                            choice.tool == "ticket_show" ||
                            choice.tool == "ticket_move" ||
                            choice.tool == "ticket_remove" ||
                            choice.tool == "ticket_create" ||
                            choice.tool == "ticket_patch")
                        {
                            // These reuse the existing run_ticket_route
                            // dispatch with an explicit intent so the
                            // legacy CRUD helpers do the actual mutation.
                            TicketIntent tint;
                            if (choice.tool == "ticket_list")   tint.kind = TicketIntent::Kind::List;
                            else if (choice.tool == "ticket_show")   tint.kind = TicketIntent::Kind::Show;
                            else if (choice.tool == "ticket_move")   tint.kind = TicketIntent::Kind::Move;
                            else if (choice.tool == "ticket_remove") tint.kind = TicketIntent::Kind::Remove;
                            else if (choice.tool == "ticket_create") tint.kind = TicketIntent::Kind::Create;
                            else                                     tint.kind = TicketIntent::Kind::Patch;
                            if (choice.args.contains("id") &&
                                choice.args["id"].is_string())
                                tint.target_id = choice.args["id"].get<std::string>();
                            if (choice.args.contains("title") &&
                                choice.args["title"].is_string())
                                tint.title = choice.args["title"].get<std::string>();
                            if (choice.args.contains("body") &&
                                choice.args["body"].is_string())
                                tint.body = choice.args["body"].get<std::string>();
                            else
                                tint.body = message;
                            if (choice.args.contains("status") &&
                                choice.args["status"].is_string())
                                tint.status = choice.args["status"].get<std::string>();
                            tint.reason = "tool_router: " + choice.tool +
                                          " (confidence " + std::to_string(choice.confidence) + ")";
                            json handler_t;
                            run_ticket_route(cwd, tint, handler_t, emit);
                            classify::Result fake_act;
                            fake_act.act     = "command";
                            fake_act.subtype = "ticket_op";
                            fake_act.tags    = { "tool-router", choice.tool };
                            json fin;
                            fin["act"]       = {{"act", fake_act.act},
                                                {"subtype", fake_act.subtype},
                                                {"tags", fake_act.tags}};
                            fin["final"]     = handler_t.value("answer", std::string{});
                            fin["handler"]   = handler_t;
                            fin["expertise"] = "ticket ops";
                            emit("final", fin);
                            hb_stop.store(true);
                            if (hb.joinable()) hb.join();
                            sink.done();
                            return false;
                        }

                        if (choice.tool == "image_gen") {
                            // Router-picked image gen. Uses the args
                            // the router already extracted (subject +
                            // optional save_to), skipping the legacy
                            // save-to regex and the greedy medium+verb
                            // detection that misfires on art-ticket
                            // bodies like "Draw a cute pixel-art robot".
                            const std::string subj =
                                choice.args.contains("subject") &&
                                choice.args["subject"].is_string()
                                    ? choice.args["subject"].get<std::string>()
                                    : message;
                            const std::string save_to =
                                choice.args.contains("save_to") &&
                                choice.args["save_to"].is_string()
                                    ? choice.args["save_to"].get<std::string>()
                                    : std::string();
                            std::string out_dir;
                            if (!cwd.empty()) {
                                out_dir = expand_home(cwd);
                                if (!out_dir.empty() && out_dir.back() != '/') out_dir.push_back('/');
                                out_dir += ".ac9_images";
                            } else {
                                const char * h = std::getenv("HOME");
                                out_dir = std::string(h ? h : "/tmp") + "/.ac9_images";
                            }
                            emit("layer", {{"name", "image_gen"},
                                           {"content", "running Chroma1-HD, subject: " + subj +
                                            (save_to.empty() ? std::string()
                                                : "\nsave to: " + save_to)}});
                            auto r = image_generator::generate(subj, out_dir);
                            json handler_e;
                            handler_e["kind"] = "image_gen";
                            handler_e["subject"] = subj;
                            std::string msg;
                            if (r.ok) {
                                std::string landed;
                                if (!save_to.empty()) {
                                    landed = image_land_at_hint(cwd, r.image_path, save_to);
                                }
                                const std::string final_path =
                                    landed.empty() ? r.image_path : landed;
                                msg = "Generated image saved to `" + final_path + "`.";
                                if (!landed.empty() && landed != r.image_path) {
                                    msg += "\n(copied from `" + r.image_path +
                                           "` per save-to hint.)";
                                }
                                handler_e["file_path"] = final_path;
                                context::append("image", "gen_path", final_path, subj);
                            } else {
                                msg = "Generation failed: " + r.message;
                                if (!r.log_tail.empty()) msg += "\n\nsd-cli log tail:\n" + r.log_tail;
                            }
                            handler_e["answer"] = msg;
                            emit("layer", {{"name", "image_gen"},
                                           {"content", msg}});
                            context::append("image", "gen", subj);
                            classify::Result fake_act;
                            fake_act.act     = "command";
                            fake_act.subtype = "image_gen";
                            fake_act.tags    = { "tool-router", "image_gen" };
                            json fin;
                            fin["act"]       = {{"act", fake_act.act},
                                                {"subtype", fake_act.subtype},
                                                {"tags", fake_act.tags}};
                            fin["final"]     = handler_e.value("answer", std::string{});
                            fin["handler"]   = handler_e;
                            fin["expertise"] = "image generation";
                            emit("final", fin);
                            hb_stop.store(true);
                            if (hb.joinable()) hb.join();
                            sink.done();
                            return false;
                        }

                        if (choice.tool == "image_edit") {
                            // Router-picked image edit. Builds an
                            // ImageIntent from the router's extracted
                            // args (edit_op + optional target_hint) and
                            // delegates to the shared run_image_edit_route
                            // helper so the resolver cascade + Chroma
                            // img2img call are identical to the legacy
                            // path.
                            ImageIntent it;
                            it.edit   = true;
                            it.reason = "tool_router: image_edit (confidence " +
                                        std::to_string(choice.confidence) + ")";
                            it.edit_op =
                                choice.args.contains("edit_op") &&
                                choice.args["edit_op"].is_string()
                                    ? choice.args["edit_op"].get<std::string>()
                                    : message;
                            // target_hint isn't a first-class ImageIntent
                            // field yet; fold it into edit_op so the
                            // resolver picks it up naturally.
                            if (choice.args.contains("target_hint") &&
                                choice.args["target_hint"].is_string())
                            {
                                const std::string th =
                                    choice.args["target_hint"].get<std::string>();
                                if (!th.empty())
                                    it.edit_op += " (target: " + th + ")";
                            }
                            std::string out_dir_e;
                            if (!cwd.empty()) {
                                out_dir_e = expand_home(cwd);
                                if (!out_dir_e.empty() && out_dir_e.back() != '/') out_dir_e.push_back('/');
                                out_dir_e += ".ac9_images";
                            } else {
                                const char * h = std::getenv("HOME");
                                out_dir_e = std::string(h ? h : "/tmp") + "/.ac9_images";
                            }
                            json handler_e;
                            run_image_edit_route(cwd, it, out_dir_e, handler_e, emit);
                            classify::Result fake_act;
                            fake_act.act     = "command";
                            fake_act.subtype = "image_edit";
                            fake_act.tags    = { "tool-router", "image_edit" };
                            json fin;
                            fin["act"]       = {{"act", fake_act.act},
                                                {"subtype", fake_act.subtype},
                                                {"tags", fake_act.tags}};
                            fin["final"]     = handler_e.value("answer", std::string{});
                            fin["handler"]   = handler_e;
                            fin["expertise"] = "image editing";
                            emit("final", fin);
                            hb_stop.store(true);
                            if (hb.joinable()) hb.join();
                            sink.done();
                            return false;
                        }

                        // mouser_search / coder / answerer / statement_note
                        // still fall through to the legacy regex routers +
                        // understanding stack. The router's opinion has
                        // been recorded on the tool_router SSE layer for
                        // observability.
                    }
                }

                // ==== EARLY ticket CRUD short-circuit (LEGACY FALLBACK) ====
                // Ticket ops are the most specific of the three deterministic
                // early routers (image / parts / ticket). "Create tickets for
                // a browser-based maze game" contains BOTH "create" and
                // "image" (the latter via "image_gen tool" or "image sprites")
                // and would otherwise route to image-gen with the whole plan
                // request as a Chroma subject. Ticket-intent detection is
                // purely deterministic keyword+regex so running it first
                // costs nothing on non-ticket turns. Kept as a safety net
                // for when the LLM router returns low confidence.
                if (!router_decided) {
                    TicketIntent tint = detect_ticket_intent(cleaned, cwd);
                    if (tint.kind != TicketIntent::Kind::None) {
                        json handler_t;
                        run_ticket_route(cwd, tint, handler_t, emit);
                        classify::Result fake_act;
                        fake_act.act     = "command";
                        fake_act.subtype = "ticket_op";
                        fake_act.tags    = { "early-ticket-route" };
                        json fin;
                        fin["act"]       = {{"act", fake_act.act},
                                            {"subtype", fake_act.subtype},
                                            {"tags", fake_act.tags}};
                        fin["final"]     = handler_t.value("answer", std::string{});
                        fin["handler"]   = handler_t;
                        fin["expertise"] = "ticket ops";
                        emit("final", fin);
                        hb_stop.store(true);
                        if (hb.joinable()) hb.join();
                        sink.done();
                        return false;
                    }
                }

                // ==== EARLY image gen / edit short-circuit (LEGACY FALLBACK) ====
                // Runs only when the LLM tool router did not decide with
                // high confidence. See router_decided flag above.
                if (!router_decided) {
                    ImageIntent gen_e  = detect_image_gen_intent(cleaned);
                    ImageIntent edit_e = detect_image_edit_intent(cleaned);
                    if (edit_e.edit && !session_has_recent_image() &&
                        !prompt_has_image_edit_hint(cleaned, cwd)) {
                        edit_e.edit = false;
                    }
                    if (gen_e.gen || edit_e.edit) {
                        std::string reason = edit_e.edit
                            ? ("edit: " + edit_e.reason)
                            : ("gen: "  + gen_e.reason);
                        emit("layer", {{"name", "image_intent"},
                                       {"content", reason +
                                        (gen_e.gen && !gen_e.subject.empty()
                                          ? "\nsubject: " + gen_e.subject
                                          : std::string()) +
                                        (edit_e.edit && !edit_e.edit_op.empty()
                                          ? "\nedit_op: " + edit_e.edit_op
                                          : std::string())}});
                        std::string out_dir_e;
                        if (!cwd.empty()) {
                            out_dir_e = expand_home(cwd);
                            if (!out_dir_e.empty() && out_dir_e.back() != '/') out_dir_e.push_back('/');
                            out_dir_e += ".ac9_images";
                        } else {
                            const char * h = std::getenv("HOME");
                            out_dir_e = std::string(h ? h : "/tmp") + "/.ac9_images";
                        }
                        json handler_e;
                        if (edit_e.edit) {
                            // Resolver cascade: explicit filename hit -> session
                            // state (existing behavior) -> vision-description
                            // match with persistent cache. Handles ambiguity
                            // + not-found paths in-band and emits the trace as
                            // an image_resolve layer.
                            run_image_edit_route(cwd, edit_e, out_dir_e,
                                                 handler_e, emit);
                        } else {
                            const std::string subj =
                                gen_e.subject.empty() ? cleaned : gen_e.subject;
                            // Honor an explicit "save to <path>" hint in
                            // the prompt so a ticket ("Draw a robot. Save
                            // to assets/robot.png.") can land the PNG
                            // exactly where the game code expects it.
                            const std::string save_hint =
                                extract_image_save_path(cleaned);
                            emit("layer", {{"name", "image_gen"},
                                           {"content", "running Chroma1-HD, subject: " + subj +
                                            (save_hint.empty() ? std::string()
                                                : "\nsave hint: " + save_hint)}});
                            auto r = image_generator::generate(subj, out_dir_e);
                            std::string msg;
                            if (r.ok) {
                                std::string landed;
                                if (!save_hint.empty()) {
                                    landed = image_land_at_hint(cwd, r.image_path, save_hint);
                                }
                                const std::string final_path =
                                    landed.empty() ? r.image_path : landed;
                                msg = "Generated image saved to `" + final_path + "`.";
                                if (!landed.empty() && landed != r.image_path) {
                                    msg += "\n(copied from `" + r.image_path +
                                           "` per save-to hint.)";
                                }
                                handler_e["file_path"] = final_path;
                                context::append("image", "gen_path", final_path, subj);
                            } else {
                                msg = "Generation failed: " + r.message;
                                if (!r.log_tail.empty()) msg += "\n\nsd-cli log tail:\n" + r.log_tail;
                            }
                            handler_e["kind"]    = "image_gen";
                            handler_e["answer"]  = msg;
                            handler_e["subject"] = subj;
                            emit("layer", {{"name", "image_gen"},
                                           {"content", msg}});
                            context::append("image", "gen", subj);
                        }
                        // Emit a synthetic final so the client wraps up the
                        // AI bubble the same way it would for the full path.
                        classify::Result fake_act;
                        fake_act.act     = "command";
                        fake_act.subtype = edit_e.edit ? "image_edit" : "image_gen";
                        fake_act.tags    = { "early-image-route" };
                        json fin;
                        fin["act"]       = {{"act", fake_act.act},
                                            {"subtype", fake_act.subtype},
                                            {"tags", fake_act.tags}};
                        fin["final"]     = handler_e.value("answer", std::string{});
                        fin["handler"]   = handler_e;
                        fin["expertise"] = edit_e.edit ? "image editing" : "image generation";
                        emit("final", fin);
                        hb_stop.store(true);
                        if (hb.joinable()) hb.join();
                        sink.done();
                        return false;
                    }
                }

                // ==== EARLY parts-search short-circuit (LEGACY FALLBACK) ====
                // Runs only when the LLM tool router did not decide with
                // high confidence. See router_decided flag above.
                if (router_decided) {
                    // no-op: the router already spoke.
                } else
                // Any prompt that names Mouser / Digi-Key, mentions
                // "in stock", or carries a manufacturer-part-number-shaped
                // token routes straight to components::extract_intent +
                // Mouser API, bypassing the whole understanding stack.
                // The stack burns 30-90 s of qwen35 per layer to sharpen
                // a query the Mouser handler will only ever keyword-search
                // — worse, its greedy sampler used to loop the stylize +
                // render_final layers on the same sentence when the input
                // contained a hyphenated part number (fixed separately in
                // coder.cpp's sampler chain). Deterministic keyword sniff
                // up top so we do NOT pay the extract_intent LLM cost on
                // unrelated turns.
                if (components::has_credentials()) {
                    auto smells_like_parts = [](const std::string & s) {
                        std::string lc; lc.reserve(s.size());
                        for (char c : s) lc.push_back(static_cast<char>(
                            std::tolower(static_cast<unsigned char>(c))));
                        if (lc.find("mouser")   != std::string::npos) return true;
                        if (lc.find("digikey")  != std::string::npos) return true;
                        if (lc.find("digi-key") != std::string::npos) return true;
                        if (lc.find("in stock") != std::string::npos) return true;
                        if (lc.find("instock")  != std::string::npos) return true;
                        // Manufacturer-part-number-shaped: >=2 hyphens,
                        // mixed digits+letters, >=6 chars, no whitespace.
                        auto looks_like_pn = [](const std::string & t) {
                            if (t.size() < 6) return false;
                            int hyphens = 0, digits = 0, alpha = 0;
                            for (char c : t) {
                                if (c == '-') ++hyphens;
                                else if (std::isdigit(static_cast<unsigned char>(c))) ++digits;
                                else if (std::isalpha(static_cast<unsigned char>(c))) ++alpha;
                                else return false;
                            }
                            return hyphens >= 2 && digits >= 1 && alpha >= 1;
                        };
                        std::string cur;
                        for (char c : s) {
                            if (std::isspace(static_cast<unsigned char>(c)) || c == ':' ||
                                c == ',' || c == '?' || c == '.' || c == ';') {
                                if (looks_like_pn(cur)) return true;
                                cur.clear();
                            } else cur.push_back(c);
                        }
                        return looks_like_pn(cur);
                    };
                    if (smells_like_parts(cleaned)) {
                        components::Intent it = components::extract_intent(cleaned);
                        emit("layer", {{"name", "parts_intent_early"},
                                       {"content",
                                        std::string("is_parts_request=") +
                                        (it.is_parts_request ? "true" : "false") +
                                        " keyword=\"" + it.keyword + "\" " + it.reasoning}});
                        if (it.is_parts_request) {
                            std::string used_keyword;
                            auto parts = components::search_with_retry(
                                it.keyword, used_keyword, /*limit=*/30, /*retries=*/3);
                            // Health analysis is applied inside search() per
                            // patch section 3; emit a structured
                            // component_health SSE layer so the AI pane can
                            // render badges + so the answer leads with any
                            // anomaly before the user's literal question.
                            int worst_rank = 0;   // 0=none 1=info 2=warn 3=critical
                            json flag_arr = json::array();
                            auto rank_of = [](const std::string & sev) {
                                if (sev == "critical") return 3;
                                if (sev == "warn")     return 2;
                                if (sev == "info")     return 1;
                                return 0;
                            };
                            for (const auto & p : parts) {
                                for (const auto & f : p.flags) {
                                    if (rank_of(f.severity) > worst_rank)
                                        worst_rank = rank_of(f.severity);
                                    flag_arr.push_back({
                                        {"mfg_part_no", p.mfg_part_no},
                                        {"code", f.code},
                                        {"severity", f.severity},
                                        {"message", f.message},
                                        {"mouser_field", f.field},
                                    });
                                }
                            }
                            const char * worst_name =
                                worst_rank == 3 ? "critical" :
                                worst_rank == 2 ? "warn"     :
                                worst_rank == 1 ? "info"     : "none";
                            emit("layer", {{"name", "component_health"},
                                           {"content", json{
                                               {"keyword", used_keyword},
                                               {"parts_analyzed", parts.size()},
                                               {"worst_severity", worst_name},
                                               {"flags", flag_arr},
                                           }.dump()}});
                            std::string a = components::format_results(
                                parts, used_keyword,
                                (it.want_full_list || it.write_to_file)
                                  ? static_cast<int>(parts.size()) : 5);
                            if (used_keyword != it.keyword) {
                                a = std::string("_(broadened search from `") +
                                    it.keyword + "` to `" + used_keyword + "`.)_\n\n" + a;
                            }
                            // Prepend health callout when anything fired,
                            // so the user sees anomalies before the count.
                            if (!flag_arr.empty()) {
                                std::string cb;
                                cb += (worst_rank == 3 ? "> [!danger]"  :
                                       worst_rank == 2 ? "> [!warning]" :
                                                          "> [!info]");
                                cb += " Component-health flags for `" +
                                      used_keyword + "`:\n";
                                for (const auto & f : flag_arr) {
                                    cb += "> - **" + f.value("code", std::string{}) +
                                          "** (Mouser " + f.value("mouser_field", std::string{}) +
                                          "): " + f.value("message", std::string{}) + "\n";
                                }
                                cb += "\n";
                                a = cb + a;
                            }
                            context::append("components", "response", a, used_keyword);
                            emit("layer", {{"name", "mouser"},
                                           {"content", std::to_string(parts.size()) +
                                                       " parts; keyword=" + used_keyword +
                                                       "; worst_severity=" + worst_name}});
                            json handler_e;
                            handler_e["kind"]    = "components_answer";
                            handler_e["keyword"] = used_keyword;
                            handler_e["answer"]  = a;
                            handler_e["worst_severity"] = worst_name;
                            handler_e["flags"]   = flag_arr;
                            classify::Result fake_act;
                            fake_act.act     = "question";
                            fake_act.subtype = "parts";
                            fake_act.tags    = { "early-parts-route" };
                            json fin;
                            fin["act"]       = {{"act", fake_act.act},
                                                {"subtype", fake_act.subtype},
                                                {"tags", fake_act.tags}};
                            fin["final"]     = a;
                            fin["handler"]   = handler_e;
                            fin["expertise"] = "electronic components (Mouser)";
                            emit("final", fin);
                            hb_stop.store(true);
                            if (hb.joinable()) hb.join();
                            sink.done();
                            return false;
                        }
                    }
                }

                classify::Result act = classify::analyze(cleaned);
                // Deterministic backstop #0: classify came back empty.
                // Observed in the maze-game run — classify emits
                // `act= subtype=` on a plain imperative CMakeLists.txt
                // request. Empty act flows through the whole downstream
                // dispatch and lands at 'no handler for act=', which
                // marks a runner ticket blocked. When the pipeline
                // ran through all the understanding-stack layers and
                // the user still typed a non-trivial prompt, default
                // to command/code so the coder gets a shot at it.
                if (act.act.empty()) {
                    act.act     = "command";
                    act.subtype = "code";
                    act.tags.push_back("empty-classify-defaulted");
                    emit("layer", {{"name", "classify"},
                                   {"content", "(empty classify → default to command/code)"}});
                }
                // Deterministic backstop for a misroute the model still
                // commits: a deontic project directive ("the webserver
                // should be in the 001_interface folder") classified as
                // statement/correction earns a "noted" and nothing
                // happens. Desired-state wording about project artifacts
                // is a command to make it so.
                if (act.act == "statement" || act.act == "correction") {
                    std::string lc;
                    lc.reserve(cleaned.size());
                    for (char c : cleaned) lc.push_back(static_cast<char>(
                        std::tolower(static_cast<unsigned char>(c))));
                    const bool directive =
                        lc.find("should")      != std::string::npos ||
                        lc.find("needs to")    != std::string::npos ||
                        lc.find("need to")     != std::string::npos ||
                        lc.find("must ")       != std::string::npos ||
                        lc.find("supposed to") != std::string::npos ||
                        lc.find("belongs")     != std::string::npos;
                    const bool artifact =
                        lc.find("folder")    != std::string::npos ||
                        lc.find("director")  != std::string::npos ||
                        lc.find("file")      != std::string::npos ||
                        lc.find("port ")     != std::string::npos ||
                        lc.find("server")    != std::string::npos ||
                        lc.find("build")     != std::string::npos ||
                        lc.find("code")      != std::string::npos ||
                        lc.find("script")    != std::string::npos ||
                        lc.find('/')         != std::string::npos ||
                        lc.find('_')         != std::string::npos ||
                        lc.find(".cpp")      != std::string::npos ||
                        lc.find(".hpp")      != std::string::npos ||
                        lc.find(".py")       != std::string::npos ||
                        lc.find(".js")       != std::string::npos;
                    if (directive && artifact) {
                        act.act     = "command";
                        act.subtype = "shell";
                        act.tags.push_back("reclassified-directive");
                    }
                }
                // Deterministic backstop #2: a plain imperative request that
                // starts with an action verb ("complete the web server, fix
                // the compile errors; verify that the project compiles")
                // classified as question sends the turn to the answerer and
                // nothing gets built. Reclassify to command/code when the
                // text opens with an action verb (not a wh-word), acts on a
                // project artifact, and does not carry question punctuation.
                if (act.act == "question") {
                    std::string lc;
                    lc.reserve(cleaned.size());
                    for (char c : cleaned) lc.push_back(static_cast<char>(
                        std::tolower(static_cast<unsigned char>(c))));
                    auto ltrim = [](const std::string & s) {
                        std::size_t i = 0;
                        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' ||
                                                s[i] == '\r' || s[i] == '\n' ||
                                                s[i] == '"' || s[i] == '\''))
                            ++i;
                        return s.substr(i);
                    };
                    const std::string head = ltrim(lc);
                    static const std::vector<std::string> kWhWords = {
                        "what",  "why",   "how",  "who",  "whom",  "whose",
                        "when",  "where", "which", "is ",  "are ",  "do ",
                        "does ", "did ",  "can ",  "could ", "should ",
                        "would ", "will ", "may ", "might ",
                    };
                    bool starts_wh = false;
                    for (const auto & w : kWhWords) {
                        if (head.size() >= w.size() &&
                            head.compare(0, w.size(), w) == 0) {
                            const char n = head.size() > w.size() ? head[w.size()] : ' ';
                            if (n == ' ' || n == '\t' || n == '?' || w.back() == ' ') {
                                starts_wh = true;
                                break;
                            }
                        }
                    }
                    static const std::vector<std::string> kAction = {
                        "create ",   "write ",    "add ",      "remove ",
                        "delete ",   "fix ",      "update ",   "change ",
                        "modify ",   "implement ","complete ", "build ",
                        "compile ",  "verify ",   "make sure ","run ",
                        "start ",    "stop ",     "restart ",  "install ",
                        "rewrite ",  "refactor ", "generate ", "produce ",
                        "output ",   "save ",     "download ", "extract ",
                        "convert ",  "replace ",  "rename ",   "move ",
                        "copy ",     "port ",     "wire ",     "hook ",
                        "finish ",   "continue ",
                    };
                    bool starts_action = false;
                    for (const auto & v : kAction) {
                        if (head.size() >= v.size() &&
                            head.compare(0, v.size(), v) == 0) {
                            starts_action = true; break;
                        }
                    }
                    const bool has_qmark =
                        cleaned.find('?') != std::string::npos;
                    const bool artifact =
                        lc.find("code")      != std::string::npos ||
                        lc.find("build")     != std::string::npos ||
                        lc.find("compil")    != std::string::npos ||
                        lc.find("server")    != std::string::npos ||
                        lc.find("file")      != std::string::npos ||
                        lc.find("folder")    != std::string::npos ||
                        lc.find("director")  != std::string::npos ||
                        lc.find("project")   != std::string::npos ||
                        lc.find("main.cpp")  != std::string::npos ||
                        lc.find(".cpp")      != std::string::npos ||
                        lc.find(".hpp")      != std::string::npos ||
                        lc.find(".py")       != std::string::npos ||
                        lc.find(".js")       != std::string::npos ||
                        lc.find("cmake")     != std::string::npos ||
                        lc.find("makefile")  != std::string::npos ||
                        lc.find("script")    != std::string::npos;
                    if (starts_action && !starts_wh && !has_qmark && artifact) {
                        act.act     = "command";
                        act.subtype = "code";
                        act.tags.push_back("reclassified-imperative");
                    }
                }
                if (bail_if_stopped()) return false;
                std::string cls = "act=" + act.act + " subtype=" + act.subtype;
                if (!act.tags.empty()) {
                    cls += " tags=";
                    for (std::size_t i = 0; i < act.tags.size(); ++i) {
                        if (i) cls += ",";
                        cls += act.tags[i];
                    }
                }
                context::append("classify", "act", act.act);
                context::append("classify", "subtype", act.subtype);
                emit("layer", {{"name", "classify"}, {"content", cls}});

                if (act.act == "acknowledgment") {
                    json fin;
                    fin["act"]       = {{"act", act.act}, {"subtype", act.subtype}, {"tags", act.tags}};
                    fin["final"]     = "(noted)";
                    fin["handler"]   = {{"kind", "noted"}};
                    fin["expertise"] = "";
                    emit("final", fin);
                    sink.done();
                    return false;
                }

                // Cross-turn reference resolution: "what do they eat there?"
                // must become "...in Paris, France?" BEFORE the KB query,
                // the lookup-intent detector, and the coder consume the
                // text; the stylize render resolves referents too, but too
                // late for those consumers. Only costs a model call when
                // the text contains a referring word and the session has
                // earlier turns.
                std::string resolved = cleaned;
                if (cleaned.size() <= 600) {
                    static const std::unordered_set<std::string> kReferers = {
                        "they", "them", "their", "theirs", "there", "it",
                        "its", "he", "she", "him", "her", "his", "hers",
                        "this", "that", "these", "those", "one",
                    };
                    bool has_ref = false;
                    for (const std::string & w : unique_words_in_order(cleaned)) {
                        if (kReferers.count(w)) { has_ref = true; break; }
                    }
                    bool has_history = false;
                    if (has_ref) {
                        const int64_t cur = context::current_turn();
                        for (const auto & r : context::by_layer("user", 3)) {
                            if (r.turn != cur && r.kind == "input") {
                                has_history = true;
                                break;
                            }
                        }
                    }
                    if (has_ref && has_history) {
                        resolved = stylize::resolve_referents(cleaned);
                        context::append("resolve", "output", resolved,
                                        resolved == cleaned ? "unchanged" : "");
                        emit("layer", {{"name", "resolve"},
                             {"content", resolved == cleaned
                                             ? "(nothing to resolve)"
                                             : resolved}});
                    }
                }

                std::string ents;
                for (const auto & m : entities::extract(resolved)) {
                    std::string row = m.original + " => " + m.canonical;
                    if (!m.summary.empty()) row += ": " + m.summary;
                    context::append("entities", "resolved", row, m.original);
                    if (!ents.empty()) ents += "\n";
                    ents += row;
                }
                emit("layer", {{"name", "entities"}, {"content", ents.empty() ? "(none)" : ents}});

                // Visible Wikipedia lookup as its own thinking layer.
                // Combine title-suggestion (good for entity names) AND
                // full-text search (good for conceptual questions). Dedupe
                // by article title. The block is kept around: it grounds
                // the answer handler (which must never re-query the KB
                // with the precision REWRITE -- titles don't match
                // glosses), and questions the local KB covers skip the
                // outbound web search entirely.
                std::string wiki_block;
                bool        kb_covered = false;
                const auto  words = unique_words_in_order(resolved);
                {
                    // Query with the CONTENT words, not the raw question:
                    // title-suggestion on "What is the capital of France?"
                    // surfaces noise (Pierre Bourdieu, Reading Capital)
                    // while "capital france" surfaces France and Paris.
                    static const std::unordered_set<std::string> kQuestionWords = {
                        "what", "which", "who", "whom", "whose", "where",
                        "why", "how", "about", "tell", "many", "much",
                        "there", "please",
                    };
                    std::string kb_query;
                    for (const std::string & w : words) {
                        if (kStopWords.count(w) || kQuestionWords.count(w)) continue;
                        if (!kb_query.empty()) kb_query.push_back(' ');
                        kb_query.append(w);
                    }
                    if (kb_query.empty()) kb_query = resolved;
                    auto sug = kb::suggest(kb_query, 3);
                    auto srh = kb::search (kb_query, 4);
                    if (!sug.available || !srh.available) {
                        wiki_block = "(knowledge offline)";
                    } else {
                        std::unordered_set<std::string> seen;
                        auto add = [&](const std::vector<kb::WikiHit> & hits) {
                            for (const auto & h : hits) {
                                if (!seen.insert(h.title).second) continue;
                                wiki_block.append("- ").append(h.title);
                                if (!h.snippet.empty()) {
                                    wiki_block.append(": ").append(h.snippet);
                                }
                                wiki_block.push_back('\n');
                            }
                        };
                        add(sug.hits);
                        add(srh.hits);
                        if (wiki_block.empty()) wiki_block = "(no matches)";
                        kb_covered = wiki_block != "(no matches)";
                    }
                    emit("layer", {{"name", "wikipedia"},
                                   {"content", "query: " + kb_query + "\n" +
                                               wiki_block}});
                }

                const std::string defs = build_stylize_defs(words);
                context::append("dictionary", "defs", defs);
                emit("layer", {{"name", "dictionary"}, {"content", defs}});

                // Thesaurus layer: Moby synonyms for the content words.
                // Local word-sense grounding for the answer handler, next
                // to the dictionary and offline-Wikipedia layers.
                std::string syns;
                for (const std::string & w : words) {
                    if (kStopWords.count(w)) continue;
                    const auto sy = dictionary::synonyms(w, 8);
                    if (sy.empty()) continue;
                    if (!syns.empty()) syns.push_back('\n');
                    syns.append(w).append(": ");
                    for (std::size_t i = 0; i < sy.size(); ++i) {
                        if (i) syns.append(", ");
                        syns.append(sy[i]);
                    }
                    if (syns.size() > 1200) break;
                }
                context::append("thesaurus", "syns", syns);
                emit("layer", {{"name", "thesaurus"},
                               {"content", syns.empty() ? "(none)" : syns}});

                // The stylize/disambiguate/render stages exist to make loose
                // prompts precise. A long detailed prompt is already precise:
                // rewriting it only loses information (and the rewrite models
                // degrade into empty output on long inputs). Skip the rewrite
                // for long inputs and hand the cleaned text straight through.
                const bool skip_rewrite = resolved.size() > 600;

                std::vector<stylize::Interpretation> interpretations;
                if (skip_rewrite) {
                    emit("layer", {{"name", "stylize"},
                                   {"content", "(skipped: long input is already precise)"}});
                } else {
                    interpretations = stylize::precise(resolved, defs);
                    std::string interp_block;
                    for (const auto & i : interpretations) {
                        const std::string lab = i.label.empty() ? "default" : i.label;
                        std::string row = "[" + lab + "] " + i.text;
                        context::append("stylize", "interpretation", row, lab);
                        if (!interp_block.empty()) interp_block += "\n";
                        interp_block += row;
                    }
                    emit("layer", {{"name", "stylize"}, {"content", interp_block}});
                }

                const std::string field = expertise::classify(resolved);
                context::append("expertise", "label", field);
                emit("layer", {{"name", "expertise"}, {"content", field}});

                std::string final_text;
                if (skip_rewrite) {
                    emit("layer", {{"name", "disambiguate"},
                                   {"content", "(skipped: long input is already precise)"}});
                } else {
                    disambiguate::Decision decision = disambiguate::decide(resolved, interpretations, field);
                    if (decision.needs_question && !interpretations.empty()) {
                        decision.needs_question = false;
                        decision.chosen_label = interpretations[0].label.empty()
                            ? std::string("default") : interpretations[0].label;
                        decision.reasoning = "auto-commit (chat v1 skips interactive questions)";
                    }
                    context::append("disambiguate", "commit", decision.chosen_label, decision.reasoning);
                    emit("layer", {{"name", "disambiguate"},
                                   {"content", "commit: " + decision.chosen_label + " — " + decision.reasoning}});
                    final_text = stylize::render_final(resolved, decision.chosen_label, defs);
                }

                // Safety net: an empty rewrite must never reach the handlers.
                if (final_text.find_first_not_of(" \t\r\n") == std::string::npos) {
                    final_text = resolved;
                }
                context::append("stylize", "final", final_text,
                                skip_rewrite ? "passthrough" : "rendered");
                emit("layer", {{"name", "render_final"}, {"content", final_text}});

                if (bail_if_stopped()) return false;

                // Text the ACTION handlers run on. The stylize render makes
                // the request precise for a human reader; executing it
                // corrupts commands (filenames dissolve into dictionary
                // glosses, verbs drift: "ethereally acceptable"). Doers get
                // the cleaned text; final_text stays for display and the
                // question/statement paths. Web-lookup augments below append
                // to BOTH so the coder still sees downloaded-resource notes.
                std::string action_text = resolved;

                // Web search hits gathered for question turns land here as a
                // labeled block that the answer handler receives out-of-band.
                // Commands still get the hits inlined into action_text (the
                // coder's prompt uses that text verbatim); questions do NOT
                // -- inlining them into final_text/resolved_reading forces
                // the answerer to ignore them per its "sense-aid only, do
                // NOT copy" rule, which is what left [answer] empty when
                // the user asked for boost alternatives.
                std::string web_block;

                json handler;

                // --- Outbound reference lookup (auto-detected, gated by
                // the project's .ac9ai.cfg web_lookup flag) ---
                // When the request would benefit from a web lookup, either
                // block with an explanatory message (flag off) or search
                // DuckDuckGo and, for commands, retrieve the resource into
                // the project and hand the coder the local path.
                bool lookup_blocked = false;
                if (act.act == "command" || act.act == "question") {
                    const bool needs_web_tag =
                        std::find(act.tags.begin(), act.tags.end(), "needs-web")
                            != act.tags.end();
                    websearch::Intent intent =
                        websearch::detect_intent(resolved, needs_web_tag);
                    // 'required' is the only verdict that can hard-block a
                    // turn. For commands, honour it only when the keyword
                    // test agrees something must be obtained: a lone model
                    // misfire once refused a hello-world page over a
                    // "needed template".
                    if (intent.required && act.act == "command" &&
                        !websearch::obtain_intent(resolved)) {
                        intent.required = false;
                    }
                    // Local sources first: a question the offline KB has
                    // material for is answered from dictionary + KB; the
                    // web is for what local sources cannot cover.
                    if (intent.needs_lookup && !intent.required &&
                        act.act == "question" && kb_covered) {
                        emit("layer", {{"name", "web lookup"},
                             {"content", "(skipped: local knowledge base has "
                                         "coverage; answering from local "
                                         "sources)"}});
                        intent.needs_lookup = false;
                    }
                    if (intent.needs_lookup) {
                        const bool allowed = project_cfg::web_lookup_enabled(cwd);
                        emit("layer", {{"name", "web lookup"},
                             {"content", "detected: " + intent.reason +
                                         "\nquery: " + intent.query +
                                         "\ntier: " +
                                         (intent.required ? "required" : "helpful") +
                                         "\nweb_lookup: " +
                                         (allowed ? "enabled" : "DISABLED")}});
                        context::append("websearch", "intent",
                                        "query=" + intent.query,
                                        allowed ? "enabled" : "disabled");
                        if (!allowed && intent.required &&
                            !(act.act == "question" && kb_covered)) {
                            // Impossible without the network: stop and say so.
                            // (Questions the local KB covers are exempt: a
                            // local answer with a staleness caveat beats a
                            // refusal.)
                            handler["kind"]   = "answer";
                            handler["answer"] =
                                "This request needs a web lookup (" + intent.reason +
                                "), but outbound web lookup is disabled for this "
                                "project.\n\nClick the globe icon in the AI pane "
                                "header to enable it" +
                                std::string(cwd.empty()
                                    ? " (open a project folder first, so the "
                                      "setting has somewhere to be saved)."
                                    : " for this project.");
                            lookup_blocked = true;
                        } else if (!allowed) {
                            // A lookup would merely help: never block local
                            // work on a disabled (or misdetected) lookup.
                            emit("layer", {{"name", "web lookup"},
                                 {"content", "(skipped: disabled for this "
                                             "project; proceeding with local "
                                             "knowledge. Globe icon enables "
                                             "lookups.)"}});
                        } else {
                            websearch::SearchResult sr =
                                websearch::search(intent.query, 8);
                            if (!sr.ok) {
                                emit("layer", {{"name", "web results"},
                                     {"content", "search failed: " + sr.error}});
                                const std::string fail_note =
                                    "\n\n(A web search for \"" + intent.query +
                                    "\" was attempted but failed: " +
                                    sr.error + ".)";
                                final_text  += fail_note;
                                action_text += fail_note;
                            } else {
                                const std::string hits_block =
                                    websearch::format_hits(sr.hits);
                                emit("layer", {{"name", "web results"},
                                     {"content", hits_block}});
                                context::append("websearch", "results",
                                                hits_block, intent.query);
                                if (act.act == "command" && intent.required) {
                                    // Something must actually be obtained.
                                    websearch::DownloadPlan plan =
                                        websearch::plan_download(cleaned, sr.hits);
                                    std::string augment;
                                    if (plan.ok && !cwd.empty()) {
                                        std::string root = cwd;
                                        if (root.back() != '/') root.push_back('/');
                                        // Land vendor/ beside a recognizable
                                        // web-root folder when there is one,
                                        // else at the project root.
                                        std::string webroot;
                                        for (const char * cand :
                                             {"001_interface", "web", "www",
                                              "public", "static", "site"}) {
                                            if (fs::exists(root + cand)) {
                                                webroot = std::string(cand) + "/";
                                                break;
                                            }
                                        }
                                        const std::string vendor =
                                            webroot + "vendor/";
                                        const std::string dest =
                                            root + vendor + plan.filename;
                                        websearch::FetchResult fr;
                                        for (const auto & u : plan.urls) {
                                            fr = websearch::fetch_to_file(u, dest);
                                            if (fr.ok && fr.size > 1024) break;
                                        }
                                        const std::string rel =
                                            vendor + plan.filename;
                                        if (fr.ok) {
                                            emit("layer", {{"name", "retrieved"},
                                                 {"content", plan.package + " -> " +
                                                  fr.path + " (" +
                                                  std::to_string(fr.size) +
                                                  " bytes)\nfrom " + fr.final_url}});
                                            context::append("websearch",
                                                "retrieved", fr.path, plan.package);
                                            // Reference the library by a path
                                            // RELATIVE to the page (no leading
                                            // slash): index.html and vendor/ both
                                            // live in the web-root folder, and the
                                            // server usually mounts that folder at
                                            // "/", so an absolute "/<folder>/..."
                                            // path would 404.
                                            const std::string page_rel =
                                                "vendor/" + plan.filename;
                                            augment =
                                                "\n\nTHE LIBRARY IS ALREADY ON DISK. "
                                                "A JavaScript library (\"" +
                                                plan.package + "\") of " +
                                                std::to_string(fr.size) +
                                                " bytes has ALREADY been downloaded "
                                                "into this project by the ac9 pipeline "
                                                "at \"" + rel + "\" (a \"vendor\" folder "
                                                "beside the web page).\n\n"
                                                "STRICTLY FORBIDDEN, do NOT do any of "
                                                "the following:\n"
                                                "  - Do NOT WRITEFILE \"" + rel + "\" "
                                                "(the real library is already there; "
                                                "writing a stub / placeholder / minimal "
                                                "shim over it will silently break the "
                                                "chart, and ac9's WRITEFILE guard will "
                                                "refuse the overwrite anyway).\n"
                                                "  - Do NOT curl, wget, or otherwise "
                                                "re-download the library from any URL, "
                                                "CDN, npm, jsDelivr, unpkg, or GitHub. "
                                                "A previous run tried this and clobbered "
                                                "the good " + std::to_string(fr.size) +
                                                "-byte file with a 94-byte error page.\n"
                                                "  - Do NOT rm the file, do NOT "
                                                "overwrite it, do NOT rename it.\n"
                                                "\nWhat you MUST do: use the file AS-IS. "
                                                "Read it with xxd -i to embed it into a "
                                                "compiled-in unsigned char array header "
                                                "if the ticket asks for embedding; "
                                                "otherwise serve it from disk. Write or "
                                                "refresh the web interface so the served "
                                                "page includes <script src=\"" +
                                                page_rel + "\"></script> (a path "
                                                "RELATIVE to the page, no leading "
                                                "slash) and a small inline script that "
                                                "demonstrates the library actually doing "
                                                "what the user asked for. The user "
                                                "EXPLICITLY asked for this library. "
                                                "Make sure the server serves both the "
                                                "page and the \"vendor\" folder from "
                                                "the same web root. The project should "
                                                "build.";
                                            if (!plan.api_hint.empty()) {
                                                augment += " Use the library's CURRENT "
                                                    "API exactly as follows and do not "
                                                    "assume an older version: " +
                                                    plan.api_hint;
                                            }
                                        } else {
                                            emit("layer", {{"name", "retrieved"},
                                                 {"content", "download failed: " +
                                                  (fr.error.empty()
                                                     ? std::string("no candidate URL worked")
                                                     : fr.error)}});
                                            augment =
                                                "\n\nWeb search results for \"" +
                                                intent.query + "\":\n" + hits_block +
                                                "\nAutomatic download failed; pick one "
                                                "library, download its standalone "
                                                "build with curl into a vendor/ "
                                                "folder, then integrate it.";
                                        }
                                    } else {
                                        augment =
                                            "\n\nWeb search results for \"" +
                                            intent.query + "\":\n" + hits_block +
                                            "\nChoose one library, download its "
                                            "standalone build with curl into a "
                                            "vendor/ folder, then integrate it.";
                                    }
                                    final_text  += augment;
                                    action_text += augment;
                                } else if (act.act == "command") {
                                    // Commands: inline the hits into the
                                    // coder's action_text (and the visible
                                    // final_text) so the shell handler sees
                                    // them alongside the request.
                                    const std::string hits_note =
                                        "\n\nWeb search results for \"" +
                                        intent.query + "\":\n" + hits_block +
                                        "\nUse these only to inform the "
                                        "commands and files you produce; "
                                        "output only shell commands and "
                                        "WRITEFILE blocks.";
                                    final_text  += hits_note;
                                    action_text += hits_note;
                                } else {
                                    // Questions: hand the hits to the answer
                                    // handler as its own WEB CONTEXT block
                                    // (see answer.hpp). Keep final_text and
                                    // action_text clean so the "sense-aid,
                                    // do NOT copy" rule around RESOLVED
                                    // READING no longer buries the source
                                    // material the model is meant to cite.
                                    web_block =
                                        "Query: " + intent.query + "\n" +
                                        hits_block;
                                }
                            }
                        }
                    }
                }

                // Parts-search short-circuit: applies to BOTH question and
                // command acts (the latter catches "find me X and write to a
                // file" — without this, the shell handler tries to scrape a
                // catalog page and usually fails). Runs only when the user
                // has a Mouser key configured. Also catches follow-ups like
                // "write it to a file" / "give me all of those" by pulling
                // the most recent components response from session memory.
                bool served_by_components = false;
                if (!lookup_blocked &&
                    (act.act == "question" || act.act == "command") &&
                    components::has_credentials())
                {
                    // Use the cleaned (de-noised) user text, not render_final,
                    // which often inflates the prompt into a verbose
                    // dictionary-style restatement that confuses keyword
                    // extraction.
                    components::Intent it = components::extract_intent(resolved);
                    emit("layer", {{"name", "parts_intent"},
                                   {"content",
                                    std::string("is_parts_request=") +
                                    (it.is_parts_request ? "true" : "false") +
                                    " use_last_results=" +
                                    (it.use_last_results ? "true" : "false") +
                                    " want_full_list=" +
                                    (it.want_full_list   ? "true" : "false") +
                                    " write_to_file=" +
                                    (it.write_to_file    ? "true" : "false") +
                                    " keyword=\"" + it.keyword + "\"" +
                                    (it.filename.empty() ? "" : " file=" + it.filename) +
                                    " " + it.reasoning}});

                    auto write_or_inline = [&](const std::string & content,
                                               int n_parts,
                                               const std::string & keyword_meta) {
                        handler["kind"] = "components_answer";
                        if (!keyword_meta.empty()) handler["keyword"] = keyword_meta;
                        if (it.write_to_file && !it.filename.empty()) {
                            std::string root = cwd.empty() ? std::string(".") : cwd;
                            std::string full =
                                root + (root.back() == '/' ? "" : "/") + it.filename;
                            std::ofstream f(full, std::ios::binary | std::ios::trunc);
                            if (f) {
                                f.write(content.data(), content.size());
                                handler["file_path"] = full;
                                handler["answer"] =
                                    "Wrote " + std::to_string(n_parts) +
                                    " parts to `" + full + "`.";
                                context::append("components", "file_written", full);
                            } else {
                                handler["answer"] =
                                    "Could not write " + full + " (open failed). "
                                    "Here are the results inline:\n\n" + content;
                            }
                        } else {
                            handler["answer"] = content;
                        }
                    };

                    if (it.is_parts_request) {
                        std::string used_keyword;
                        auto parts = components::search_with_retry(
                            it.keyword, used_keyword, /*limit=*/30, /*retries=*/3);
                        const int rows = (it.want_full_list || it.write_to_file)
                                           ? static_cast<int>(parts.size())
                                           : 5;
                        std::string a = components::format_results(parts, used_keyword, rows);
                        if (used_keyword != it.keyword) {
                            a = std::string("_(broadened search from `") +
                                it.keyword + "` to `" + used_keyword +
                                "` — initial query had no hits.)_\n\n" + a;
                        }
                        context::append("components", "response", a, used_keyword);
                        write_or_inline(a, static_cast<int>(parts.size()), used_keyword);
                        emit("layer", {{"name", "mouser"},
                                       {"content", std::to_string(parts.size()) +
                                                   " parts; keyword=" + used_keyword +
                                                   (used_keyword != it.keyword
                                                     ? " (was: " + it.keyword + ")"
                                                     : "")}});
                        served_by_components = true;
                    } else if (it.use_last_results) {
                        // Pull the most recent components/response row from the
                        // current session so follow-ups work conversationally.
                        auto rows = context::by_layer("components", 25);
                        std::string last;
                        std::string last_keyword;
                        for (const auto & r : rows) {
                            if (r.kind == "response") {
                                last         = r.content;
                                last_keyword = r.meta;
                                break;
                            }
                        }
                        if (last.empty()) {
                            handler["kind"]   = "components_answer";
                            handler["answer"] =
                                "No previous parts list in this session to reuse. "
                                "Ask for a search first (e.g. \"find a 3.3V "
                                "switching regulator, 1A, in stock\").";
                            served_by_components = true;
                        } else {
                            // Approx part count = number of \n\n separated rows
                            // after the header; cheaper than re-querying.
                            int approx = 0;
                            for (std::size_t i = 0; i + 1 < last.size(); ++i)
                                if (last[i] == '\n' && last[i+1] == '\n') ++approx;
                            // If the user asked for the FULL list and the cached
                            // version is truncated (has "more — ask" hint), we
                            // need to re-search and emit everything. Re-run.
                            if (it.want_full_list &&
                                last.find("more — ask") != std::string::npos &&
                                !last_keyword.empty())
                            {
                                auto parts = components::search(last_keyword, 30);
                                last = components::format_results(
                                    parts, last_keyword,
                                    static_cast<int>(parts.size()));
                                approx = static_cast<int>(parts.size());
                                context::append("components", "response", last, last_keyword);
                            }
                            write_or_inline(last, approx, last_keyword);
                            emit("layer", {{"name", "mouser"},
                                           {"content", "reused last results"
                                                       " (keyword=" + last_keyword + ")"}});
                            served_by_components = true;
                        }
                    }
                }

                if (bail_if_stopped()) return false;

                // -- Image gen / edit short-circuit --
                // Runs BEFORE the act dispatch so "generate a picture of a
                // fluffy kitty" doesn't get misrouted to the shell coder
                // (classifier tends to see command/code and dispatch it
                // straight through). Deterministic keyword-based intent
                // detection; short-circuits with a routed-to notice from
                // the currently-stubbed image_generator / image_editor
                // modules. When the underlying models get wired up, only
                // the handler body inside this branch needs to change.
                bool served_by_image = false;
                if (!lookup_blocked && !served_by_components) {
                    ImageIntent gen  = detect_image_gen_intent(resolved);
                    ImageIntent edit = detect_image_edit_intent(resolved);
                    // Edit intent needs a prior image_gen/edit in the
                    // session to be trusted; otherwise "make it work" and
                    // similar phrases would keep hitting this branch.
                    // Exception: an explicit filename token or a project
                    // with images + a target-noun in the prompt is signal
                    // enough — the resolver can then find the image even
                    // without a prior session turn.
                    if (edit.edit && !session_has_recent_image() &&
                        !prompt_has_image_edit_hint(resolved, cwd)) {
                        edit.edit = false;
                    }
                    if (gen.gen || edit.edit) {
                        std::string reason;
                        if (edit.edit) reason = "edit: " + edit.reason;
                        else           reason = "gen: "  + gen.reason;
                        emit("layer", {{"name", "image_intent"},
                                       {"content", reason +
                                        (gen.gen && !gen.subject.empty()
                                          ? "\nsubject: " + gen.subject
                                          : std::string()) +
                                        (edit.edit && !edit.edit_op.empty()
                                          ? "\nedit_op: " + edit.edit_op
                                          : std::string())}});
                        // Choose an output directory. Prefer the open project
                        // under `cwd` so the user sees the PNG land in the
                        // file tree; fall back to a per-session cache in
                        // .ac9_images/ under HOME when no project is open.
                        std::string out_dir;
                        if (!cwd.empty()) {
                            out_dir = expand_home(cwd);
                            if (!out_dir.empty() && out_dir.back() != '/') out_dir.push_back('/');
                            out_dir += ".ac9_images";
                        } else {
                            const char * h = std::getenv("HOME");
                            out_dir = std::string(h ? h : "/tmp") + "/.ac9_images";
                        }
                        if (edit.edit) {
                            // Resolver cascade — see run_image_edit_route
                            // for details. Same helper as the early
                            // image-intent short-circuit above.
                            run_image_edit_route(cwd, edit, out_dir,
                                                 handler, emit);
                        } else {
                            const std::string subj =
                                gen.subject.empty() ? resolved : gen.subject;
                            emit("layer", {{"name", "image_gen"},
                                           {"content", "running Chroma1-HD, subject: " + subj}});
                            auto r = image_generator::generate(subj, out_dir);
                            std::string msg;
                            if (r.ok) {
                                msg = "Generated image saved to `" + r.image_path + "`.";
                                handler["file_path"] = r.image_path;
                                context::append("image", "gen_path", r.image_path, subj);
                            } else {
                                msg = "Generation failed: " + r.message;
                                if (!r.log_tail.empty()) msg += "\n\nsd-cli log tail:\n" + r.log_tail;
                            }
                            handler["kind"]    = "image_gen";
                            handler["answer"]  = msg;
                            handler["subject"] = subj;
                            emit("layer", {{"name", "image_gen"},
                                           {"content", msg}});
                            context::append("image", "gen", subj);
                        }
                        served_by_image = true;
                    }
                }

                if (lookup_blocked) {
                    // handler already holds the "lookup disabled" message.
                } else if (served_by_components) {
                    // handler is already filled in; skip the act-based dispatch.
                } else if (served_by_image) {
                    // handler is already filled in; skip the act-based dispatch.
                } else if (act.act == "command") {
                    // The coder is stateless across turns; without the
                    // earlier requests it re-decides the stack from scratch
                    // and turn-1 constraints ("C++", "port 9001", "resources
                    // compiled into the binary") silently vanish by turn 8.
                    // Hand it the session's prior user inputs, oldest first,
                    // capped so the coder's 8k context still fits the survey
                    // and any inlined files.
                    std::string history;
                    {
                        const int64_t cur_turn = context::current_turn();
                        std::vector<std::string> items;
                        for (const auto & r : context::by_layer("user", 50)) {
                            if (r.turn == cur_turn || r.kind != "input") continue;
                            items.push_back(r.content);
                        }
                        // by_layer is newest-first; flip to chronological.
                        std::reverse(items.begin(), items.end());
                        // Pasted build logs are stubbed hard; detection
                        // needs real log markers, not the bare word "make"
                        // ("make the cmakelists generic" is a constraint,
                        // not a log).
                        auto shrink = [](std::string t, std::size_t cap) {
                            std::string tl = t;
                            for (char & ch : tl) ch = static_cast<char>(
                                std::tolower(static_cast<unsigned char>(ch)));
                            const bool loggish = t.size() > 1000 &&
                                (tl.find("error:")    != std::string::npos ||
                                 tl.find("make: ***") != std::string::npos ||
                                 tl.find("collect2")  != std::string::npos ||
                                 tl.find("undefined reference")
                                     != std::string::npos);
                            if (loggish) cap = 160;
                            if (t.size() > cap) { t.resize(cap); t += " ..."; }
                            return t;
                        };
                        // The FIRST turn usually carries the project
                        // constraints and they sit deep in it ("port 9001"
                        // was at char 471 of the session this rescues), so
                        // it gets a cap that fits a whole spec paragraph;
                        // it is kept always, plus the most recent turns.
                        std::vector<std::string> keep;
                        if (!items.empty()) {
                            keep.push_back(shrink(items.front(), 800));
                        }
                        for (std::size_t i =
                                 items.size() > 5 ? items.size() - 4 : 1;
                             i < items.size(); ++i) {
                            keep.push_back(shrink(items[i], 400));
                        }
                        std::size_t total = 0;
                        for (const auto & t : keep) {
                            if (total + t.size() > 3200) break;
                            total += t.size();
                            history.append("- ").append(t).push_back('\n');
                        }
                    }
                    // "Fix the build" requests run the compile-fix loop:
                    // build, hand the errors to the coder, apply, rebuild;
                    // only a genuinely passing build reports success.
                    // Both paths act on the de-noised text, never the render
                    // (the rewrite mangles 'fix'/'build'/'compile' into
                    // synonyms and dissolves filenames on short prompts).
                    const auto loop_note =
                        [&](const std::string & n, const std::string & c) {
                            context::append("shell", "loop", c, n);
                            emit("layer", {{"name", n}, {"content", c}});
                        };
                    // Compound requests get a task plan: the classifier
                    // tags them multi-part (with fallbacks), and one giant
                    // coder call on "remove X; create Y; build Z inside
                    // it; verify it compiles" has invented parallel build
                    // systems and dropped the verify clause.
                    std::string lc_cmd;
                    lc_cmd.reserve(resolved.size());
                    for (char c : resolved) lc_cmd.push_back(static_cast<char>(
                        std::tolower(static_cast<unsigned char>(c))));
                    const bool verify_clause =
                        lc_cmd.find("compil")      != std::string::npos ||
                        lc_cmd.find("verify")      != std::string::npos ||
                        lc_cmd.find("build error") != std::string::npos;
                    const bool creationish =
                        lc_cmd.find("create")    != std::string::npos ||
                        lc_cmd.find("write ")    != std::string::npos ||
                        lc_cmd.find("start a")   != std::string::npos ||
                        lc_cmd.find("add ")      != std::string::npos ||
                        lc_cmd.find("implement") != std::string::npos ||
                        lc_cmd.find("build a")   != std::string::npos ||
                        lc_cmd.find("make a ")   != std::string::npos;
                    const bool multi_part =
                        std::find(act.tags.begin(), act.tags.end(),
                                  "multi-part") != act.tags.end() ||
                        std::count(resolved.begin(), resolved.end(), ';') >= 3 ||
                        (creationish && verify_clause);
                    // Order matters: a creation request ending in "make
                    // sure it compiles and fix any errors" must NOT be
                    // stolen by the build-fix matcher; on an empty project
                    // fix_build falls back to a single shot and the verify
                    // clause is silently dropped. The plan runs its verify
                    // step through the build loop at the end instead.
                    const auto sh = multi_part
                        ? shell_tool::execute_plan(action_text, cwd, history,
                                                   loop_note)
                        : (!creationish && shell_tool::is_build_fix(resolved))
                        ? shell_tool::fix_build(resolved, cwd, loop_note,
                                                history)
                        : shell_tool::execute(action_text, cwd, history);
                    const std::string shown_cwd =
                        cwd.empty() ? std::string("(server default)") : cwd;
                    context::append("shell", "command", sh.command, shown_cwd);
                    context::append("shell", "output",  sh.stdout_text,
                                    "exit=" + std::to_string(sh.exit_code));
                    handler["kind"]      = "shell";
                    handler["command"]   = sh.command;
                    handler["stdout"]    = sh.stdout_text;
                    handler["exit_code"] = sh.exit_code;
                    handler["cwd"]       = shown_cwd;

                    // Post-code comment pass: re-run the SAME loaded
                    // coder on each written file with a Doxygen-comment
                    // system prompt. Preserves executable code; only
                    // inserts doc/inline comments. Emitted as its own
                    // "comment" SSE layer BEFORE the shell layer so the
                    // AI pane reads coder -> comment -> shell -> final.
                    if (std::getenv("AC9_COMMENT_PASS") == nullptr ||
                        std::string(std::getenv("AC9_COMMENT_PASS")) != "0") {
                        std::vector<std::string> commented;
                        for (const auto & path : sh.written_files) {
                            status::pulse();
                            std::ifstream src(path);
                            if (!src) continue;
                            std::stringstream buf; buf << src.rdbuf();
                            const std::string source = buf.str();
                            if (source.empty()) continue;
                            std::string ext;
                            if (auto d = path.find_last_of('.'); d != std::string::npos)
                                ext = path.substr(d + 1);
                            const std::string lang =
                                (ext == "cpp" || ext == "cxx" || ext == "cc" ||
                                 ext == "hpp" || ext == "hxx" || ext == "hh"  ||
                                 ext == "h"   || ext == "c") ? "C++" :
                                (ext == "py")               ? "Python" :
                                (ext == "js" || ext == "ts")? "JavaScript/TypeScript" :
                                                              "source";
                            bool ctrunc = false;
                            std::string annotated;
                            try {
                                annotated = coder::comment_code(
                                    lang, source,
                                    /*max_new_tokens=*/6144, &ctrunc);
                            } catch (const std::exception & ex) {
                                std::fprintf(stderr,
                                    "comment: coder threw for %s: %s\n",
                                    path.c_str(), ex.what());
                                continue;
                            }
                            // Defensive fence strip on the annotated
                            // output: coder::comment_code frequently
                            // wraps the whole file in ```lang fences
                            // ("```cmake\n...```") which the ofstream
                            // below would land in the real source file,
                            // making cmake / gcc parse-error on the
                            // fence line. Strip leading `^```lang?\n`
                            // and trailing `\n```` before the write.
                            {
                                static const std::regex ao(R"(^\s*```[A-Za-z0-9+._\-]*[ \t]*\r?\n)");
                                static const std::regex ac(R"(\r?\n\s*```\s*$)");
                                annotated = std::regex_replace(annotated, ao, "");
                                annotated = std::regex_replace(annotated, ac, "");
                            }
                            // The annotated file must be at least as
                            // long as the original (comments only add
                            // bytes). If shorter/truncated, keep the
                            // raw source: never lose code to a bad
                            // annotation pass.
                            if (annotated.size() < source.size() || ctrunc) {
                                std::fprintf(stderr,
                                    "comment: keeping raw %s (annotated=%zu raw=%zu trunc=%d)\n",
                                    path.c_str(),
                                    annotated.size(), source.size(),
                                    (int) ctrunc);
                                continue;
                            }
                            std::ofstream out(path, std::ios::binary | std::ios::trunc);
                            if (!out) {
                                std::fprintf(stderr,
                                    "comment: failed to open %s for write\n",
                                    path.c_str());
                                continue;
                            }
                            out << annotated;
                            commented.push_back(path);
                        }
                        if (!commented.empty()) {
                            std::string summary =
                                "Doxygen + inline comment pass over " +
                                std::to_string(commented.size()) +
                                " file(s):\n";
                            for (const auto & p : commented)
                                summary += "  " + p + "\n";
                            emit("layer", {{"name",    "comment"},
                                           {"content", summary}});
                            context::append("shell", "comment", summary, "");
                        }
                    }

                    emit("layer", {{"name", "shell"},
                                   {"content", "cwd: " + shown_cwd + "\n" +
                                               sh.command + "\n" + sh.stdout_text +
                                               "\n[exit " + std::to_string(sh.exit_code) + "]"}});
                } else if (act.act == "question") {
                    // Domain routing by expertise label:
                    //   chemistry → ChemLLM-20B on GPU 1
                    //   physics   → Qwen3-14B on GPU 1
                    //   else      → general answer handler on GPU 0
                    std::string field_lc;
                    field_lc.reserve(field.size());
                    for (char c : field) field_lc.push_back(
                        static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
                    const bool is_chem    = field_lc.find("chem") != std::string::npos;
                    const bool is_physics = !is_chem &&
                        field_lc.find("physics") != std::string::npos;
                    // Electronics / electrical / embedded → Qwen3-14B (the
                    // same model physics uses). Same broad academic
                    // reasoning + good EE coverage in training data.
                    const bool is_electronics = !is_chem && !is_physics && (
                        field_lc.find("electronic") != std::string::npos ||
                        field_lc.find("electrical") != std::string::npos ||
                        field_lc.find("embedded")   != std::string::npos ||
                        field_lc.find("circuit")    != std::string::npos);
                    const char * which = is_chem        ? "chemistry"
                                       : is_physics     ? "physics"
                                       : is_electronics ? "electronics"
                                       : "answer";
                    // (parts-search short-circuit already handled above for
                    // both question and command acts; if we're here, the
                    // prompt isn't a parts request.)
                    // Local grounding for the general answerer: dictionary
                    // senses plus Moby synonyms (the KB block rides in its
                    // own parameter).
                    std::string local_defs = defs;
                    if (!syns.empty()) {
                        local_defs.append("\n\nSYNONYMS (Moby thesaurus):\n")
                                  .append(syns);
                    }
                    std::string a;
                    if      (is_chem)        a = chemistry::answer(final_text);
                    else if (is_physics)     a = physics::answer  (final_text);
                    else if (is_electronics) a = physics::answer  (final_text);
                    else                     a = answer::respond  (resolved,
                                                                   wiki_block,
                                                                   local_defs,
                                                                   final_text,
                                                                   web_block);
                    context::append("answer", "response", a, which);
                    handler["kind"]   = std::string(which) + "_answer";
                    handler["answer"] = a;
                    emit("layer", {{"name", which}, {"content", a}});
                } else if (act.act == "statement" || act.act == "correction") {
                    // Content corrections ("no, her name is Pamela") land
                    // here too: recorded into memory rather than falling
                    // through to "no handler for act=correction".
                    bool persistent = false;
                    for (const auto & t : act.tags) if (t == "persistent") persistent = true;
                    const std::string msg = statement::ingest(cleaned, final_text, persistent);
                    handler["kind"]    = "statement";
                    handler["message"] = msg;
                    emit("layer", {{"name", "statement"}, {"content", msg}});
                } else {
                    handler["kind"]    = "none";
                    handler["message"] = "no handler for act=" + act.act;
                }

                if (bail_if_stopped()) return false;

                json fin;
                fin["act"]       = {{"act", act.act}, {"subtype", act.subtype}, {"tags", act.tags}};
                fin["final"]     = final_text;
                fin["handler"]   = handler;
                fin["expertise"] = field;
                emit("final", fin);
            } catch (const std::exception & ex) {
                emit("error", json{{"error", ex.what()}});
            }
            hb_stop.store(true);
            if (hb.joinable()) hb.join();
            sink.done();
            return false;
        }
    );
}

}

void stop() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_srv) g_srv->stop();
    g_running.store(false);
}

void run(const std::string & host, int port) {
    {
        std::lock_guard<std::mutex> lk(g_mtx);
        g_srv = std::make_unique<httplib::Server>();
    }
    // Register the LLM-based tool router's default tools once at
    // startup so /api/chat can dispatch through it.
    tool_router::init();
    httplib::Server & srv = *g_srv;
    srv.set_payload_max_length(64 * 1024 * 1024);  // up to 64MB writes
    // Streaming endpoints (chat pipeline, terminal exec_stream) can idle
    // for minutes between emits. httplib's 5-second default write timeout
    // aborts the chunked provider mid-stream, which the client sees as an
    // early `exit` event and the prompt returning while `watch` or a
    // running server is still going. Ten minutes is long enough to cover
    // any realistic PTY quiet window and short enough that a genuinely
    // dead connection still gets cleaned up.
    srv.set_read_timeout (600, 0);
    srv.set_write_timeout(600, 0);
    // Keep the connection alive across the many sequential fetch calls
    // the UI makes (fs list, stat, scan, tree_hash, chat, terminal).
    srv.set_keep_alive_max_count(1000);
    srv.set_keep_alive_timeout  (30);
    srv.new_task_queue = []{
        return new httplib::ThreadPool(8);
    };

    // No per-request access log on stderr; errors only (see set_error_handler).

    // -- /api/* JSON endpoints --
    srv.Get ("/api/status",        handle_status);
    // Live system + per-GPU stats for the AI-pane ring widgets. Cached
    // 250 ms server-side so a 2 Hz client poll stays cheap.
    srv.Get ("/api/gpu_stats",
            [](const httplib::Request &, httplib::Response & res) {
        nlohmann::json out;
        try {
            auto sys = hardware::query_system_stats();
            nlohmann::json sj;
            sj["mem_used"]  = sys.mem_used;
            sj["mem_total"] = sys.mem_total;
            sj["cpu"]       = sys.cpu_pct;
            sj["temp"]      = sys.temp_c;
            sj["n_cpus"]    = sys.n_cpus;
            out["system"]   = std::move(sj);
        } catch (...) { out["system"] = nlohmann::json::object(); }
        out["gpus"] = nlohmann::json::array();
        try {
            for (const auto & g : hardware::query_gpu_stats()) {
                nlohmann::json gj;
                gj["id"]        = g.id;
                gj["name"]      = g.name;
                gj["temp"]      = g.temp_c;
                gj["util"]      = g.util_pct;
                gj["mem_used"]  = g.mem_used;
                gj["mem_total"] = g.mem_total;
                gj["role"]      = g.role;
                out["gpus"].push_back(std::move(gj));
            }
        } catch (...) {}
        res.set_header("Cache-Control", "no-store");
        res.set_content(out.dump(), "application/json");
    });
    // Role -> short_name map + the actively selected role for each
    // pipeline stage. Client uses this to display "thinking (Q3 Think 30)"
    // instead of a generic "thinking..." based on which layer is firing.
    srv.Get ("/api/models_map", [](const httplib::Request &,
                                    httplib::Response & res) {
        nlohmann::json out;
        out["shorts"] = nlohmann::json::object();
        std::ifstream f("data/manifest.json");
        if (f) {
            try {
                nlohmann::json m; f >> m;
                for (auto it = m.begin(); it != m.end(); ++it) {
                    if (it.value().is_object() &&
                        it.value().contains("short_name")) {
                        out["shorts"][it.key()] = it.value()["short_name"];
                    }
                }
            } catch (...) {}
        }
        // Also pull short_names from sources.json for roles whose chunks
        // are not on disk yet (helps the UI label planner-30b before we
        // ever load it).
        std::ifstream sf("data/sources.json");
        if (sf) {
            try {
                nlohmann::json s; sf >> s;
                for (auto it = s.begin(); it != s.end(); ++it) {
                    if (it.value().is_object() &&
                        it.value().contains("short_name") &&
                        !out["shorts"].contains(it.key())) {
                        out["shorts"][it.key()] = it.value()["short_name"];
                    }
                }
            } catch (...) {}
        }
        // Fallback short-names for roles that don't have manifest entries
        // yet (single source of truth once the manifest catches up).
        static const std::pair<const char *, const char *> kBuiltinShorts[] = {
            {"coder",       "Qwen2.5-Coder-14BI"},
            {"coder-14b",   "Qwen2.5-Coder-14BI"},
            {"coder-big",   "Qwen3-Coder-30B"},
            {"qwen35",      "Qwen3.6-35B Claude-4.7"},
            {"qwen14b",     "Qwen2.5-14BI"},
            {"physics",     "Qwen3-14B"},
            {"vision",      "Qwen2.5-VL-7B"},
            {"cleanup",     "Qwen1.5-1.5B"},
            {"dictionary",  "Wordnet"},
            {"safety",      "Llama-Guard-3-1B"},
            {"image_gen",   "Chroma1-HD"},
            {"image_edit",  "Qwen-Image-Edit"},
        };
        for (const auto & [k, v] : kBuiltinShorts) {
            if (!out["shorts"].contains(k)) out["shorts"][k] = v;
        }
        out["active"] = nlohmann::json::object();
        auto env = [](const char * k, const char * dflt) {
            const char * v = std::getenv(k);
            return std::string(v && *v ? v : dflt);
        };
        // Consolidated: planning + thinking + coding all now run on the
        // single resident qwen35 instance (Qwen3.6-35B-A3B Claude-4.7-Opus
        // abliterated, layer-split across both P100s, ~47 TPS). The old
        // planner-4b / planner-30b / coder / coder-big roles remain
        // reachable via AC9_{PLANNER,CODER}_ROLE for A/B, but the default
        // that the AI-pane headline reads is qwen35 in both slots so the
        // UI stops labeling the active run as a 14B / 4B model when
        // qwen35 is actually generating.
        out["active"]["planner"]       = env("AC9_PLANNER_ROLE", "qwen35");
        out["active"]["coder"]         = env("AC9_CODER_ROLE",   "qwen35");
        // Understanding stack (classify / entities / expertise / stylize /
        // disambiguate / render_final / resolve / answer / …) now also
        // routes to qwen35: modules/003_stylize/qwen14b.cpp delegates
        // qwen14b::generate to coder::generate under the hood, so every
        // historic 14B call runs on the same resident 2-card qwen35
        // instance the coder + planner use.
        out["active"]["understanding"] = "qwen35";
        out["active"]["cleanup"]       = "cleanup";
        res.set_header("Cache-Control", "no-store");
        res.set_content(out.dump(), "application/json");
    });
    srv.Get ("/api/fs/list",       handle_fs_list);
    srv.Get ("/api/fs/read",       handle_fs_read);
    srv.Get ("/api/fs/raw",        handle_fs_raw);
    srv.Get ("/api/fs/stat",       handle_fs_stat);
    srv.Post("/api/fs/scan",       handle_fs_scan);
    srv.Get ("/api/fs/tree_hash",  handle_fs_tree_hash);
    srv.Post("/api/fs/mkdir",      handle_fs_mkdir);
    srv.Post("/api/fs/write",      handle_fs_write);
    srv.Post("/api/fs/write_raw",  handle_fs_write_raw);
    srv.Post("/api/fs/rename",     handle_fs_rename);
    srv.Delete("/api/fs/delete",   handle_fs_delete);

    // -- .tickets.agile board endpoints --
    srv.Get   ("/api/tickets",                     handle_tickets_list);
    srv.Post  ("/api/tickets/init",                handle_tickets_init);
    srv.Post  ("/api/tickets/create",              handle_tickets_create);
    srv.Get   (R"(/api/tickets/(T-\d+))",          handle_tickets_get);
    srv.Patch (R"(/api/tickets/(T-\d+))",          handle_tickets_patch);
    srv.Delete(R"(/api/tickets/(T-\d+))",          handle_tickets_delete);
    srv.Post  (R"(/api/tickets/(T-\d+)/move)",     handle_tickets_move);
    // Ticket runner: hammer button drives this loop.
    srv.Post  ("/api/tickets/run/start",           handle_tickets_run_start);
    srv.Post  ("/api/tickets/run/stop",            handle_tickets_run_stop);
    srv.Get   ("/api/tickets/run/status",          handle_tickets_run_status);
    srv.Get   ("/api/tickets/run/events",          handle_tickets_run_events);
    srv.Post  ("/api/tickets/repair",              handle_tickets_repair);

    // GET /api/project/config?cwd=<root> -> per-project .ac9ai.cfg state.
    srv.Get("/api/project/config", [](const httplib::Request & req, httplib::Response & res) {
        const std::string cwd = expand_home(req.get_param_value("cwd"));
        json j{
            {"has_project", !cwd.empty()},
            {"web_lookup",  project_cfg::web_lookup_enabled(cwd)},
            {"path",        project_cfg::cfg_path(cwd)},
        };
        res.set_content(j.dump(), "application/json");
    });

    // POST /api/project/config {cwd, web_lookup} -> write and echo state.
    srv.Post("/api/project/config", [](const httplib::Request & req, httplib::Response & res) {
        json body = json::parse(req.body, nullptr, false);
        const std::string cwd = expand_home(
            body.is_object() ? body.value("cwd", std::string{}) : std::string{});
        if (cwd.empty()) {
            res.status = 400;
            res.set_content(R"({"error":"no project open"})", "application/json");
            return;
        }
        const bool want = body.is_object() && body.value("web_lookup", false);
        if (!project_cfg::set_web_lookup(cwd, want)) {
            res.status = 500;
            res.set_content(R"({"error":"could not write .ac9ai.cfg"})", "application/json");
            return;
        }
        json j{
            {"has_project", true},
            {"web_lookup",  project_cfg::web_lookup_enabled(cwd)},
            {"path",        project_cfg::cfg_path(cwd)},
        };
        res.set_content(j.dump(), "application/json");
    });

    // POST /api/vision  {path: "...", prompt: "..."}  -> {text: "..."}
    // Loads the Qwen3-VL model on demand (evicts coder/physics/chemistry on
    // GPU 1), runs the image + prompt through it, returns the description.
    srv.Post("/api/vision", [](const httplib::Request & req, httplib::Response & res) {
        json body = json::parse(req.body, nullptr, false);
        if (!body.is_object() || !body.contains("path")) {
            res.status = 400;
            res.set_content(R"({"error":"missing path"})", "application/json");
            return;
        }
        const std::string p   = expand_home(body["path"].get<std::string>());
        const std::string pr  = body.value("prompt",
                                           std::string("Describe what is shown in this image."));
        try {
            std::string out = vision::describe(p, pr);
            json j{{"text", out}};
            res.set_content(j.dump(), "application/json");
        } catch (const std::exception & e) {
            res.status = 500;
            json j{{"error", e.what()}};
            res.set_content(j.dump(), "application/json");
        }
    });
    srv.Post("/api/terminal/exec",        handle_terminal_exec);
    srv.Post("/api/terminal/exec_stream", handle_terminal_exec_stream);
    srv.Post("/api/terminal/complete",    handle_terminal_complete);
    // ---- persistent PTY per tab (real terminal via xterm.js) ----
    srv.Post("/api/terminal/open",        handle_terminal_open);
    srv.Get ("/api/terminal/stream",      handle_terminal_stream);
    srv.Post("/api/terminal/write",       handle_terminal_write);
    srv.Post("/api/terminal/resize",      handle_terminal_resize);
    srv.Post("/api/terminal/close",       handle_terminal_close);
    srv.Post("/api/chat",          handle_chat);
    // POST /api/chat/stop -> cancel the active chat turn (UI stop
    // button). The running pipeline notices per generated token and
    // between stages, deletes the turn's session rows so the prompt
    // never replays, and emits a "stopped" SSE event to its client.
    srv.Post("/api/chat/stop", [](const httplib::Request &, httplib::Response & res) {
        status::request_cancel();
        res.set_content(R"({"ok":true})", "application/json");
    });
    // GET /api/chat/status?sid=<session_id> -> {running: bool, seq: N}
    // A reloaded page uses this to decide whether to synthesize an
    // in-flight AI-pane bubble before opening /api/chat/events.
    srv.Get("/api/chat/status",
            [](const httplib::Request & req, httplib::Response & res) {
        const std::string sid = req.get_param_value("sid");
        nlohmann::json out{{"running", false}, {"seq", 0}};
        if (!sid.empty()) {
            std::shared_ptr<ChatRun> run;
            {
                std::lock_guard<std::mutex> lk(g_chat_runs_mu);
                auto it = g_chat_runs.find(sid);
                if (it != g_chat_runs.end()) run = it->second;
            }
            if (run) {
                out["running"] = run->running.load();
                std::lock_guard<std::mutex> lk(run->subs_mu);
                out["seq"] = run->next_seq > 0 ? run->next_seq - 1 : 0;
                out["server_session"] = std::to_string(g_session_id);
            }
        }
        res.set_content(out.dump(), "application/json");
    });
    // GET /api/chat/events?sid=<session_id>&since=<sess>-<seq>
    // SSE stream that mirrors the frames sent via the POST /api/chat
    // response for that session. Used when a browser tab is reloaded
    // mid-turn: the original POST fetch is gone but the pipeline is
    // still running server-side, so a fresh subscriber can pick up
    // any events still in the ring PLUS every future frame until the
    // pipeline exits. Same seq-cursor protocol as ticket run events.
    srv.Get("/api/chat/events",
            [](const httplib::Request & req, httplib::Response & res) {
        const std::string sid = req.get_param_value("sid");
        if (sid.empty()) {
            res.status = 400;
            res.set_content(R"({"error":"missing sid"})", "application/json");
            return;
        }
        std::shared_ptr<ChatRun> run;
        {
            std::lock_guard<std::mutex> lk(g_chat_runs_mu);
            auto it = g_chat_runs.find(sid);
            if (it == g_chat_runs.end()) {
                // Mint an idle placeholder so a later POST turn can
                // reuse it and its subscribers survive the mint.
                run = std::make_shared<ChatRun>();
                run->sid = sid;
                g_chat_runs[sid] = run;
            } else {
                run = it->second;
            }
        }
        uint64_t client_session = 0, client_seq = 0;
        auto parse_cursor = [&](const std::string & s) {
            auto dash = s.find('-');
            if (dash == std::string::npos) return;
            try {
                client_session = std::stoull(s.substr(0, dash));
                client_seq     = std::stoull(s.substr(dash + 1));
            } catch (...) { client_session = 0; client_seq = 0; }
        };
        if (req.has_header("Last-Event-ID"))
            parse_cursor(req.get_header_value("Last-Event-ID"));
        if (client_session == 0 && req.has_param("since"))
            parse_cursor(req.get_param_value("since"));
        res.set_chunked_content_provider(
            "text/event-stream",
            [run, client_session, client_seq](std::size_t,
                                              httplib::DataSink & sink) -> bool {
                auto sub = std::make_shared<ChatRun::Subscriber>();
                sub->alive.store(true);
                auto write_mu = std::make_shared<std::mutex>();
                sub->write = [&sink, write_mu](const std::string & frame) -> bool {
                    std::lock_guard<std::mutex> lk(*write_mu);
                    return sink.write(frame.data(), frame.size());
                };
                // Emit a server-session header so the client can detect
                // that the server has restarted (id mismatch = ring is
                // meaningless, wipe the pane and start fresh).
                {
                    const std::string hello =
                        "event: session\ndata: {\"id\":\"" +
                        std::to_string(g_session_id) + "\"}\n\n";
                    std::lock_guard<std::mutex> lk(*write_mu);
                    if (!sink.write(hello.data(), hello.size())) return false;
                }
                {
                    std::lock_guard<std::mutex> lk(run->subs_mu);
                    const bool same_session = (client_session == g_session_id);
                    for (const auto & item : run->ring) {
                        if (same_session && item.seq <= client_seq) continue;
                        if (!sub->write(item.frame)) {
                            sub->alive.store(false); break;
                        }
                    }
                    if (sub->alive.load()) run->subs.push_back(sub);
                }
                // Keepalive tick while the subscription is live. The
                // pipeline path (chat_run_publish) writes real frames
                // directly via sub->write from whichever thread is
                // running the turn.
                while (sub->alive.load()) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::lock_guard<std::mutex> lk(*write_mu);
                    if (!sink.write(": ka\n\n", 6)) {
                        sub->alive.store(false);
                        break;
                    }
                }
                sink.done();
                return false;
            });
    });
    srv.Post("/api/context/clear", [](const httplib::Request & req, httplib::Response & r) {
        const std::string sid = req.get_header_value("X-Tool-Session");
        if (!sid.empty() && sessions_store::exists(sid) && sid != context::current_id()) {
            try { context::switch_to(sid); } catch (...) {}
        }
        // Reuse the current session if it's already empty. Every mint
        // via context::new_session used to plant a nameless picker
        // ghost (context::new_session writes only the .sqlite, no
        // metadata blob; sessions_store::list() directory-scans and
        // picks up the ghost). If the currently-active session has
        // zero turn rows there's nothing meaningful to preserve; just
        // stay on it and report ok.
        const std::string cur = context::current_id();
        if (cur.empty() || sessions_store::message_count(cur) > 0) {
            context::new_session();
        }
        json j{{"ok", true}, {"id", context::current_id()}};
        r.set_content(j.dump(), "application/json");
    });

    // ---- Browser-session management ------------------------------------
    // GET  /api/sessions                  -> {sessions:[{id,name,...}]}
    // POST /api/sessions {name?,root_dir?}-> {id, ...meta}
    // GET  /api/sessions/<id>             -> {ui:{...}, chat:[{role,text,...}]}
    // PUT  /api/sessions/<id>             -> save {ui:{...}}
    // PATCH /api/sessions/<id>            -> rename / set folder
    // POST /api/sessions/<id>/activate    -> make this the active session
    // DELETE /api/sessions/<id>           -> forget (delete json + sqlite)
    // GET /api/sessions/active -> {id: "<uuid>" | ""}
    // The id of the session whose root_dir matches an active tickets
    // run's cwd, or "" if there is no unambiguous pick. Lets the client
    // skip the picker on cold-start when there is live work to resume.
    srv.Get("/api/sessions/active", [](const httplib::Request &, httplib::Response & res) {
        std::vector<std::string> running_cwds;
        {
            std::lock_guard<std::mutex> lk(g_ticket_runs_mu);
            for (const auto & [cwd, run] : g_ticket_runs) {
                if (run && run->running.load()) running_cwds.push_back(cwd);
            }
        }
        std::string pick;
        for (const auto & m : sessions_store::list()) {
            for (const auto & cwd : running_cwds) {
                if (m.root_dir == cwd) { pick = m.id; break; }
            }
            if (!pick.empty()) break;
        }
        json j{ {"id", pick} };
        res.set_header("Cache-Control", "no-store");
        res.set_content(j.dump(), "application/json");
    });

    srv.Get("/api/sessions", [](const httplib::Request &, httplib::Response & res) {
        json arr = json::array();
        const std::string active = context::current_id();
        for (const auto & m : sessions_store::list()) {
            arr.push_back({
                { "id",            m.id            },
                { "name",          m.name          },
                { "root_dir",      m.root_dir      },
                { "created_at",    m.created_at    },
                { "last_active",   m.last_active   },
                { "message_count", m.message_count },
                { "active",        m.id == active  },
            });
        }
        res.set_content(json{{"sessions", arr},
                             {"active",   active}}.dump(),
                        "application/json");
    });
    srv.Post("/api/sessions", [](const httplib::Request & req, httplib::Response & res) {
        json body = json::parse(req.body, nullptr, false);
        std::string name, root;
        if (body.is_object()) {
            name = body.value("name",     std::string{});
            root = body.value("root_dir", std::string{});
        }
        auto m = sessions_store::create(name, root);
        json j{
            { "id",            m.id            },
            { "name",          m.name          },
            { "root_dir",      m.root_dir      },
            { "created_at",    m.created_at    },
            { "last_active",   m.last_active   },
            { "message_count", 0               },
        };
        res.set_content(j.dump(), "application/json");
    });

    // Path-param helpers: extract the id from the URL.
    auto id_from_path = [](const httplib::Request & req) -> std::string {
        if (req.matches.size() >= 2) return req.matches[1].str();
        return {};
    };

    srv.Get(R"(/api/sessions/([0-9a-f-]+))",
        [id_from_path](const httplib::Request & req, httplib::Response & res) {
            std::string id = id_from_path(req);
            if (!sessions_store::exists(id)) {
                res.status = 404;
                res.set_content(R"({"error":"not found"})", "application/json");
                return;
            }
            std::string ui_blob = sessions_store::read_ui(id);
            json ui = json::parse(ui_blob, nullptr, false);
            if (!ui.is_object()) ui = json::object();
            // Rebuild the chat the way the live UI rendered it: user rows
            // plus, per AI turn, the layer chain and the handler that
            // produced the headline. The client feeds each AI entry through
            // the same renderer the live SSE path uses.
            json chat = json::array();
            {
                json ai;
                bool ai_open = false;
                int64_t ai_turn = -1;
                std::string sh_cmd, sh_cwd, sh_out, sh_exit;
                std::string ans_kind, ans_text;
                auto flush_ai = [&]() {
                    if (!ai_open) return;
                    json handler = json::object();
                    if (!sh_cmd.empty()) {
                        std::string shl = "cwd: " +
                            (sh_cwd.empty() ? std::string("(server default)") : sh_cwd) +
                            "\n" + sh_cmd;
                        if (!sh_out.empty() || !sh_exit.empty()) {
                            shl += "\n" + sh_out + "\n[exit " +
                                   (sh_exit.empty() ? std::string("?") : sh_exit) + "]";
                        }
                        ai["layers"].push_back({{"name", "shell"}, {"content", shl}});
                        handler["kind"]      = "shell";
                        handler["command"]   = sh_cmd;
                        handler["stdout"]    = sh_out;
                        handler["exit_code"] = sh_exit.empty() ? 0 : std::atoi(sh_exit.c_str());
                        handler["cwd"]       = sh_cwd;
                    } else if (!ans_kind.empty()) {
                        handler["kind"] = ans_kind;
                        if (ans_kind == "statement") handler["message"] = ans_text;
                        else                         handler["answer"]  = ans_text;
                    }
                    ai["handler"] = handler;
                    chat.push_back(ai);
                    ai_open = false;
                    sh_cmd.clear(); sh_cwd.clear(); sh_out.clear(); sh_exit.clear();
                    ans_kind.clear(); ans_text.clear();
                };
                for (const auto & r : sessions_store::turn_rows(id)) {
                    if (r.layer == "user" && r.kind == "input") {
                        flush_ai();
                        chat.push_back({{"role", "user"}, {"text", r.content}, {"ts", r.ts}});
                        ai_turn = r.turn;
                        continue;
                    }
                    if (!ai_open || r.turn != ai_turn) {
                        flush_ai();
                        ai_open = true;
                        ai_turn = r.turn;
                        ai = json{{"role", "ai"}, {"ts", r.ts}, {"layers", json::array()}};
                    }
                    if (r.layer == "shell") {
                        if (r.kind == "command") { sh_cmd = r.content; sh_cwd = r.meta; }
                        else if (r.kind == "output") {
                            sh_out = r.content;
                            if (r.meta.rfind("exit=", 0) == 0) sh_exit = r.meta.substr(5);
                        } else if (r.kind == "loop") {
                            // build-fix loop entries; meta holds the stage name
                            ai["layers"].push_back({
                                {"name", r.meta.empty() ? std::string("build") : r.meta},
                                {"content", r.content}});
                        }
                        continue;
                    }
                    const bool ends_answer = r.layer.size() >= 6 &&
                        r.layer.compare(r.layer.size() - 6, 6, "answer") == 0;
                    if (r.kind == "response" && (ends_answer || r.layer == "components")) {
                        ans_kind = (r.layer == "components") ? "components_answer" : r.layer;
                        ans_text = r.content;
                        continue;
                    }
                    if (r.layer == "statement" && r.kind == "echo") {
                        ans_kind = "statement";
                        ans_text = r.content;
                        continue;
                    }
                    std::string name    = r.layer;
                    std::string content = r.content;
                    if (r.layer == "stylize" && r.kind == "final") {
                        name = "render_final";
                        ai["final"] = content;
                    } else if (r.layer == "disambiguate" && !r.meta.empty()) {
                        content = "commit: " + content + " — " + r.meta;
                    }
                    ai["layers"].push_back({{"name", name}, {"content", content}});
                }
                flush_ai();
            }
            res.set_content(json{
                { "id",   id   },
                { "ui",   ui   },
                { "chat", chat },
            }.dump(), "application/json");
        });

    srv.Put(R"(/api/sessions/([0-9a-f-]+))",
        [id_from_path](const httplib::Request & req, httplib::Response & res) {
            std::string id = id_from_path(req);
            json body = json::parse(req.body, nullptr, false);
            if (!body.is_object() || !body.contains("ui")) {
                res.status = 400;
                res.set_content(R"({"error":"missing ui"})", "application/json");
                return;
            }
            sessions_store::write_ui(id, body["ui"].dump());
            res.set_content(R"({"ok":true})", "application/json");
        });

    srv.Patch(R"(/api/sessions/([0-9a-f-]+))",
        [id_from_path](const httplib::Request & req, httplib::Response & res) {
            std::string id = id_from_path(req);
            json body = json::parse(req.body, nullptr, false);
            std::string name, root;
            if (body.is_object()) {
                name = body.value("name",     std::string{});
                root = body.value("root_dir", std::string{});
            }
            sessions_store::patch(id, name, root);
            res.set_content(R"({"ok":true})", "application/json");
        });

    srv.Post(R"(/api/sessions/([0-9a-f-]+)/activate)",
        [id_from_path](const httplib::Request & req, httplib::Response & res) {
            std::string id = id_from_path(req);
            if (!sessions_store::exists(id)) {
                res.status = 404;
                res.set_content(R"({"error":"not found"})", "application/json");
                return;
            }
            try {
                context::switch_to(id);
                sessions_store::touch(id);
            } catch (const std::exception & e) {
                res.status = 500;
                res.set_content(json{{"error", e.what()}}.dump(),
                                "application/json");
                return;
            }
            res.set_content(R"({"ok":true})", "application/json");
        });

    srv.Delete(R"(/api/sessions/([0-9a-f-]+))",
        [id_from_path](const httplib::Request & req, httplib::Response & res) {
            std::string id = id_from_path(req);
            bool was_active = (id == context::current_id());
            // If we're about to delete the active session, hop to another
            // existing one (or create a fresh one) first so we don't leave
            // the server holding a closed/deleted file handle.
            if (was_active) {
                std::string next_id;
                for (const auto & m : sessions_store::list()) {
                    if (m.id != id) { next_id = m.id; break; }
                }
                if (!next_id.empty()) {
                    try { context::switch_to(next_id); } catch (...) {}
                } else {
                    context::new_session();
                }
            }
            sessions_store::forget(id);
            res.set_content(R"({"ok":true})", "application/json");
        });

    // GET  /api/settings -> returns stored API credentials JSON (or {} if none).
    // POST /api/settings -> overwrites settings/credentials.json with the
    //                       JSON body. Used by component-lookup and future
    //                       API-driven tools (Mouser, Digi-Key, etc.).
    srv.Get("/api/settings", [](const httplib::Request &, httplib::Response & res) {
        const std::string path = "settings/credentials.json";
        if (!fs::is_regular_file(path)) {
            res.set_content("{}", "application/json");
            return;
        }
        std::ifstream f(path);
        std::stringstream ss; ss << f.rdbuf();
        std::string body = ss.str();
        if (body.empty()) body = "{}";
        res.set_content(body, "application/json");
    });
    srv.Post("/api/settings", [](const httplib::Request & req, httplib::Response & res) {
        // Validate it's parseable JSON before writing.
        auto j = json::parse(req.body, nullptr, false);
        if (j.is_discarded()) {
            res.status = 400;
            res.set_content(R"({"error":"invalid JSON"})", "application/json");
            return;
        }
        fs::create_directories("settings");
        std::ofstream f("settings/credentials.json", std::ios::trunc);
        f << j.dump(2);
        res.set_content(R"({"ok":true})", "application/json");
    });
    // POST /api/download {url, cwd?, filename?}
    // Server-side fetch via curl that writes into <cwd>/Downloads/<basename>.
    // AutoClank 9001 runs on the user's server in the basement; pulling resources
    // through the user's workstation browser would land downloads on the
    // wrong machine. This endpoint keeps datasheets / schematic symbols /
    // any URL the user picks on the same box as the project.
    srv.Post("/api/download", [](const httplib::Request & req, httplib::Response & res) {
        json body = json::parse(req.body, nullptr, false);
        if (!body.is_object() || !body.contains("url")) {
            res.status = 400;
            res.set_content(R"({"error":"missing url"})", "application/json");
            return;
        }
        const std::string url       = body["url"].get<std::string>();
        const std::string cwd_in    = expand_home(body.value("cwd",      std::string{}));
        const std::string suggested = body.value("filename", std::string{});
        const std::string root      = cwd_in.empty() ? std::string(".") : cwd_in;
        const std::string ddir      = root + (root.back() == '/' ? "" : "/") + "Downloads";

        std::error_code ec;
        fs::create_directories(ddir, ec);
        if (ec) {
            res.status = 500;
            res.set_content(json{{"error", "mkdir " + ddir + ": " + ec.message()}}.dump(),
                            "application/json");
            return;
        }

        // Derive a sensible filename: prefer caller's suggestion, else
        // basename from the URL (stripped of ?query / #fragment).
        std::string fname = suggested;
        if (fname.empty()) {
            std::string clean_url = url;
            auto cut = clean_url.find_first_of("?#");
            if (cut != std::string::npos) clean_url.resize(cut);
            auto slash = clean_url.find_last_of('/');
            if (slash != std::string::npos) fname = clean_url.substr(slash + 1);
        }
        if (fname.empty()) fname = "download.bin";

        // Sanitize: keep [A-Za-z0-9._-], collapse anything else to '_'.
        std::string clean_fname;
        for (char c : fname) {
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_')
                clean_fname.push_back(c);
            else if (!clean_fname.empty() && clean_fname.back() != '_')
                clean_fname.push_back('_');
        }
        if (clean_fname.empty() || clean_fname == ".") clean_fname = "download.bin";

        // If file already exists, suffix with -1, -2, … so we don't clobber.
        std::string full = ddir + "/" + clean_fname;
        if (fs::exists(full)) {
            std::string stem = clean_fname;
            std::string ext;
            auto dot = stem.find_last_of('.');
            if (dot != std::string::npos && dot > 0) {
                ext  = stem.substr(dot);
                stem = stem.substr(0, dot);
            }
            for (int i = 1; i < 1000; ++i) {
                std::string candidate = ddir + "/" + stem + "-" + std::to_string(i) + ext;
                if (!fs::exists(candidate)) { full = candidate; break; }
            }
        }

        FILE * f = std::fopen(full.c_str(), "wb");
        if (!f) {
            res.status = 500;
            res.set_content(json{{"error", "fopen " + full}}.dump(),
                            "application/json");
            return;
        }

        CURL * c = curl_easy_init();
        if (!c) {
            std::fclose(f);
            std::error_code rec;
            fs::remove(full, rec);
            res.status = 500;
            res.set_content(R"({"error":"curl init failed"})", "application/json");
            return;
        }
        char errbuf[CURL_ERROR_SIZE] = {0};
        curl_easy_setopt(c, CURLOPT_URL,            url.c_str());
        curl_easy_setopt(c, CURLOPT_WRITEFUNCTION,
            +[](char * ptr, std::size_t s, std::size_t n, void * ud) -> std::size_t {
                return std::fwrite(ptr, s, n, static_cast<FILE *>(ud));
            });
        curl_easy_setopt(c, CURLOPT_WRITEDATA,      f);
        curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
        // Some CDNs (notably the Mouser datasheet host) return 403 to the
        // default libcurl UA; a real-browser UA gets through.
        curl_easy_setopt(c, CURLOPT_USERAGENT,
            "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/130.0.0.0 Safari/537.36");
        curl_easy_setopt(c, CURLOPT_ERRORBUFFER,    errbuf);
        curl_easy_setopt(c, CURLOPT_TIMEOUT,        180L);
        curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 20L);

        CURLcode rc = curl_easy_perform(c);
        long http_code = 0;
        curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);
        char * eff_url = nullptr;
        curl_easy_getinfo(c, CURLINFO_EFFECTIVE_URL, &eff_url);
        std::string final_url = eff_url ? eff_url : url;
        curl_easy_cleanup(c);
        std::fclose(f);

        if (rc != CURLE_OK) {
            std::error_code rec;
            fs::remove(full, rec);
            res.status = 502;
            res.set_content(json{{"error",
                std::string("curl: ") + (errbuf[0] ? errbuf : curl_easy_strerror(rc))}}.dump(),
                "application/json");
            return;
        }
        if (http_code < 200 || http_code >= 300) {
            std::error_code rec;
            fs::remove(full, rec);
            res.status = 502;
            res.set_content(json{{"error", "http " + std::to_string(http_code)},
                                 {"url", final_url}}.dump(),
                            "application/json");
            return;
        }

        std::error_code sec;
        auto sz = fs::file_size(full, sec);
        res.set_content(json{
            {"path",       full},
            {"size",       sec ? 0 : static_cast<int64_t>(sz)},
            {"final_url",  final_url},
        }.dump(), "application/json");
    });

    srv.Post("/api/quit", [](const httplib::Request &, httplib::Response & r) {
        r.set_content(R"({"ok":true})", "application/json");
        std::thread([]{ web_server::stop(); }).detach();
    });

    // -- static assets (wildcard, last) --
    srv.Get(R"(/.*)", [](const httplib::Request & req, httplib::Response & res) {
        const auto * a = interface_assets::find(req.path);
        if (!a) {
            res.status = 404;
            res.set_content("not found", "text/plain");
            return;
        }
        // Assets are baked into the binary at build time; when the user
        // rebuilds `AutoClank 9001` we want the browser to pick up the fresh
        // app.js / app.css / index.html on the very next request. Without
        // this the old JS keeps running until the user hard-refreshes,
        // which is how a stale ANSI regex kept leaking `(B` sequences
        // after a fix was already deployed.
        res.set_header("Cache-Control", "no-store");
        res.set_content(reinterpret_cast<const char *>(a->data), a->size, a->mime);
    });

    srv.set_error_handler([](const httplib::Request & req, httplib::Response & res) {
        std::fprintf(stderr, "[http %d] %s %s\n",
                     res.status, req.method.c_str(), req.path.c_str());
        char buf[64];
        std::snprintf(buf, sizeof(buf), "error %d\n", res.status);
        res.set_content(buf, "text/plain");
    });

    g_running.store(true);
    // Expose the actual bound port so the ticket runner's internal
    // httplib::Client can call back into /api/chat on the loopback.
    g_local_port.store(port);
    std::fprintf(stderr, "ac9: web ui listening on http://%s:%d\n", host.c_str(), port);
    srv.listen(host.c_str(), port);
    g_running.store(false);
    std::fprintf(stderr, "tool: web server stopped\n");
}

}
