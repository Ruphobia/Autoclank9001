// SPDX-License-Identifier: GPL-3.0-or-later
#include "office_suite.hpp"

#include "../010_interface/httplib.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <random>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

// Collabora Online (coolwsd) supervisor. Rules of engagement:
//
//  - No fake success: if the binary is missing, init() records exactly
//    what path was probed and how the operator should install the
//    runtime, then returns. Status remains {ready=false, enabled=false}
//    and the front-end's Office tabs display the reason.
//
//  - Adopt-if-already-listening: if the port already answers
//    /hosting/discovery with 200, treat it as owned externally (child
//    pid stays -1) so multiple ac9 launches during dev don't stomp on
//    each other. shutdown() then leaves it alone.
//
//  - Log everything: coolwsd's stdio is redirected to
//    ~/.ac9/office-suite.log (append mode) so the operator has a
//    single place to look when the doc iframe stays blank.

namespace office_suite {
namespace fs = std::filesystem;

namespace {

std::mutex        g_mtx;
Status            g_status;
pid_t             g_child_pid   = -1;   // -1 = we did not spawn (missing / adopted / disabled)
int               g_port        = 0;
std::string       g_docs_dir;
std::string       g_url_override;
std::string       g_token;
std::string       g_binary;
std::string       g_runtime_dir;
std::string       g_lo_template;
std::string       g_fs_root;
std::string       g_log_path;
std::atomic<bool> g_started_probe{false};
std::atomic<bool> g_shutting_down{false};

// ---------- small utils --------------------------------------------------

std::string env_or(const char * name, const std::string & dflt) {
    const char * v = std::getenv(name);
    return (v && *v) ? std::string(v) : dflt;
}

std::string expand_home(std::string p) {
    if (!p.empty() && p[0] == '~') {
        const char * home = std::getenv("HOME");
        return std::string(home ? home : "/") + p.substr(1);
    }
    return p;
}

std::string hex_random(std::size_t bytes) {
    // 64 hex chars from /dev/urandom for the WOPI access token. Falls
    // back to std::random_device if /dev/urandom is unreadable (should
    // never happen on Linux but be defensive).
    std::vector<unsigned char> raw(bytes);
    bool ok = false;
    int fd = ::open("/dev/urandom", O_RDONLY | O_CLOEXEC);
    if (fd >= 0) {
        std::size_t got = 0;
        while (got < raw.size()) {
            ssize_t n = ::read(fd, raw.data() + got, raw.size() - got);
            if (n <= 0) break;
            got += static_cast<std::size_t>(n);
        }
        ::close(fd);
        ok = (got == raw.size());
    }
    if (!ok) {
        std::random_device rd;
        for (auto & b : raw) b = static_cast<unsigned char>(rd() & 0xff);
    }
    static const char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve(bytes * 2);
    for (unsigned char b : raw) {
        out.push_back(kHex[b >> 4]);
        out.push_back(kHex[b & 0xf]);
    }
    return out;
}

bool port_alive(int port) {
    // Probe /hosting/discovery on the loopback. httplib::Client uses a
    // short connect timeout so this returns fast when the port is dead.
    try {
        httplib::Client cli("127.0.0.1", port);
        cli.set_connection_timeout(0, 500 * 1000);  // 0.5 s
        cli.set_read_timeout(2, 0);
        auto res = cli.Get("/hosting/discovery");
        return res && res->status == 200;
    } catch (...) {
        return false;
    }
}

void ensure_parent_dir(const std::string & path) {
    std::error_code ec;
    fs::path p(path);
    fs::create_directories(p.parent_path(), ec);
}

void set_detail(const std::string & msg) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_status.detail = msg;
}

// ---------- discovery-based readiness poll -------------------------------

void probe_loop() {
    // Poll /hosting/discovery for up to 60 seconds after spawn. Once
    // 200 is seen, mark ready and exit. If we hit the deadline without
    // a 200, leave ready=false but keep the process running -- the
    // operator can retry a browser refresh once coolwsd finally warms
    // up (large-model / cold-cache first starts have been observed
    // near the 40 s mark on this box).
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(90);
    while (!g_shutting_down.load()) {
        if (port_alive(g_port)) {
            std::lock_guard<std::mutex> lk(g_mtx);
            g_status.ready  = true;
            g_status.detail = "coolwsd ready on :" + std::to_string(g_port);
            return;
        }
        if (std::chrono::steady_clock::now() > deadline) {
            std::lock_guard<std::mutex> lk(g_mtx);
            g_status.detail =
                "coolwsd did not answer /hosting/discovery within 90 s; "
                "check " + g_log_path;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// ---------- fork+exec ----------------------------------------------------

// Returns pid on success, -1 on failure. Sets detail on failure.
pid_t spawn_coolwsd() {
    // Argument list mirrors the operator's demo invocation, minus the
    // filesystem-path options that are now pinned via config-file /
    // --o:* overrides so ac9 can retarget the runtime dir via env.
    std::vector<std::string> args = {
        g_binary,
        "--config-file=" + g_runtime_dir + "/config/coolwsd.xml",
        "--lo-template-path=" + g_lo_template,
        "--disable-cool-user-checking",
        // Keep the hack flags from the demo: --noseccomp and --nocaps
        // let coolwsd start under the operator's plain user account
        // without CAP_NET_ADMIN / CAP_SYS_ADMIN. libnss_wrapper is
        // wired via env (LD_PRELOAD + NSS_WRAPPER_*) below.
        "--noseccomp",
        "--nocaps",
        "--o:sys_template_path=" + g_runtime_dir + "/systemplate",
        "--o:child_root_path=" + g_runtime_dir + "/child-roots",
        "--o:file_server_root_path=" + g_fs_root,
        "--o:cache_files.path=" + g_runtime_dir + "/cache",
        // Coolwsd's own file logger writes into the runtime dir; we
        // capture stdout/stderr instead (see child dup2 below) so the
        // operator has one log to tail.
        "--o:logging.file[@enable]=false",
    };

    std::vector<char *> argv;
    argv.reserve(args.size() + 1);
    for (auto & s : args) argv.push_back(const_cast<char *>(s.c_str()));
    argv.push_back(nullptr);

    ensure_parent_dir(g_log_path);
    int log_fd = ::open(g_log_path.c_str(),
                        O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC,
                        0644);
    if (log_fd < 0) {
        set_detail("could not open log file " + g_log_path + ": " +
                   std::strerror(errno));
        return -1;
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        int e = errno;
        ::close(log_fd);
        set_detail(std::string("fork failed: ") + std::strerror(e));
        return -1;
    }
    if (pid == 0) {
        // Child. Detach from ac9's process group so a Ctrl-C on the
        // terminal only signals ac9; our own shutdown() forwards
        // SIGTERM explicitly. Then redirect stdio to the log file.
        ::setpgid(0, 0);
        ::dup2(log_fd, STDOUT_FILENO);
        ::dup2(log_fd, STDERR_FILENO);
        ::close(log_fd);
        // libnss_wrapper: point NSS at fake passwd/group files that
        // exist under the runtime dir so coolwsd's chroot lookups do
        // not need real /etc/passwd entries. Only wire it if the
        // wrapper files exist; otherwise skip so we do not break a
        // system that has coolwsd's proper packaging installed.
        const std::string pw = g_runtime_dir + "/nss_wrapper_passwd";
        const std::string gr = g_runtime_dir + "/nss_wrapper_group";
        if (fs::exists(pw) && fs::exists(gr)) {
            ::setenv("NSS_WRAPPER_PASSWD", pw.c_str(), 1);
            ::setenv("NSS_WRAPPER_GROUP",  gr.c_str(), 1);
            // libnss_wrapper.so has different SONAMEs on Debian and
            // Fedora. Let the operator override via env; otherwise try
            // both common paths.
            const char * pre = std::getenv("LD_PRELOAD");
            std::string preload = pre ? pre : "";
            if (preload.find("libnss_wrapper") == std::string::npos) {
                const char * cand[] = {
                    "libnss_wrapper.so",
                    "libnss_wrapper.so.0",
                };
                for (const char * c : cand) {
                    if (!preload.empty()) preload += ":";
                    preload += c;
                    break;
                }
                ::setenv("LD_PRELOAD", preload.c_str(), 1);
            }
        }
        ::execv(g_binary.c_str(), argv.data());
        // execv only returns on failure; write directly to the log
        // fd (already dup'd to fd 2) and exit.
        std::fprintf(stderr,
                     "ac9-office-suite: execv %s failed: %s\n",
                     g_binary.c_str(), std::strerror(errno));
        _exit(127);
    }
    // Parent.
    ::close(log_fd);
    return pid;
}

}  // namespace

// ---------- public API ---------------------------------------------------

void init(int coolwsd_port, const std::string & docs_dir_) {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (g_started_probe.load()) return;  // idempotent

    g_port     = coolwsd_port;
    g_docs_dir = docs_dir_;
    g_token    = hex_random(32);

    // Resolve env-driven paths. Defaults match the operator's known
    // layout; every one is overridable so a fresh box can point at a
    // different install without recompiling.
    g_binary       = env_or("AC9_COLLABORA_BINARY",
                             "/home/jwoods/work/collabora-prefix/usr/bin/coolwsd");
    g_runtime_dir  = env_or("AC9_OFFICE_SUITE_RUNTIME",
                             "/home/jwoods/work/cool-runtime");
    g_lo_template  = env_or("AC9_OFFICE_SUITE_LOTEMPLATE",
                             "/home/jwoods/work/collabora-prefix/opt/collaboraoffice");
    g_fs_root      = env_or("AC9_OFFICE_SUITE_FSROOT",
                             "/home/jwoods/work/collabora-prefix/usr/share/coolwsd");
    g_log_path     = expand_home(env_or("AC9_OFFICE_SUITE_LOG",
                                        "~/.ac9/office-suite.log"));
    g_url_override = env_or("AC9_OFFICE_SUITE_URL", "");

    // Disabled by operator? Note it and bail.
    const char * flag = std::getenv("AC9_OFFICE_SUITE");
    if (flag && (std::string(flag) == "0" || std::string(flag) == "false")) {
        g_status.enabled = false;
        g_status.ready   = false;
        g_status.detail  = "office_suite disabled via AC9_OFFICE_SUITE=0";
        std::fprintf(stderr, "ac9: office_suite disabled (AC9_OFFICE_SUITE=0)\n");
        g_started_probe.store(true);
        return;
    }

    // Ensure the docs dir exists.
    std::error_code ec;
    fs::create_directories(g_docs_dir, ec);

    // Binary present?
    if (!fs::exists(g_binary)) {
        g_status.enabled = false;
        g_status.ready   = false;
        g_status.detail  =
            "coolwsd binary not found at " + g_binary +
            ". Install the Collabora Online runtime (see modules/1720_office_suite/README.md) "
            "or set AC9_COLLABORA_BINARY.";
        std::fprintf(stderr,
                     "ac9: !!!! office_suite unavailable: %s !!!!\n",
                     g_status.detail.c_str());
        g_started_probe.store(true);
        return;
    }

    // Something already listening?
    if (port_alive(coolwsd_port)) {
        g_status.enabled = true;
        g_status.ready   = true;
        g_status.detail  =
            "adopted external coolwsd already listening on :" +
            std::to_string(coolwsd_port);
        g_child_pid = -1;  // sentinel: we do not own it
        std::fprintf(stderr, "ac9: %s\n", g_status.detail.c_str());
        g_started_probe.store(true);
        return;
    }

    // Fork+exec the supervised child.
    g_status.enabled = true;
    g_status.ready   = false;
    g_status.detail  = "starting coolwsd on :" + std::to_string(coolwsd_port);
    std::fprintf(stderr,
                 "ac9: launching coolwsd (%s) on :%d, log -> %s\n",
                 g_binary.c_str(), coolwsd_port, g_log_path.c_str());
    pid_t pid = spawn_coolwsd();
    if (pid < 0) {
        g_status.enabled = false;
        g_status.ready   = false;
        std::fprintf(stderr,
                     "ac9: !!!! office_suite failed to start: %s !!!!\n",
                     g_status.detail.c_str());
        g_started_probe.store(true);
        return;
    }
    g_child_pid = pid;
    std::fprintf(stderr, "ac9: coolwsd pid=%d\n", (int)pid);

    g_started_probe.store(true);
    // Poll /hosting/discovery in the background so status() flips to
    // ready without blocking ac9's own listen().
    std::thread(probe_loop).detach();
}

void shutdown() {
    g_shutting_down.store(true);
    pid_t pid;
    {
        std::lock_guard<std::mutex> lk(g_mtx);
        pid = g_child_pid;
        g_child_pid = -1;
    }
    if (pid <= 0) return;
    // SIGTERM then wait up to 10 s for a clean exit; SIGKILL as backup.
    ::kill(pid, SIGTERM);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (std::chrono::steady_clock::now() < deadline) {
        int stat = 0;
        pid_t r = ::waitpid(pid, &stat, WNOHANG);
        if (r == pid) return;
        if (r < 0 && errno == ECHILD) return;  // already reaped
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    ::kill(pid, SIGKILL);
    int stat = 0;
    ::waitpid(pid, &stat, 0);
}

Status status() {
    // Refresh the ready bit opportunistically; a caller polling this
    // every second is cheap enough for a local httplib probe.
    if (g_started_probe.load() && g_status.enabled) {
        bool alive = port_alive(g_port);
        std::lock_guard<std::mutex> lk(g_mtx);
        if (alive && !g_status.ready) {
            g_status.ready  = true;
            g_status.detail = "coolwsd ready on :" + std::to_string(g_port);
        } else if (!alive && g_status.ready) {
            g_status.ready  = false;
            g_status.detail = "coolwsd stopped answering /hosting/discovery";
        }
    }
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_status;
}

std::string coolwsd_url() {
    std::lock_guard<std::mutex> lk(g_mtx);
    if (!g_url_override.empty()) return g_url_override;
    // Prefer the machine's actual hostname so remote browsers can reach
    // the port; fall back to localhost if gethostname() is unhelpful.
    char host[256];
    if (::gethostname(host, sizeof(host)) == 0 && host[0]) {
        return "http://" + std::string(host) + ":" + std::to_string(g_port);
    }
    return "http://localhost:" + std::to_string(g_port);
}

std::string docs_dir() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_docs_dir;
}

std::string access_token() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_token;
}

bool enabled() {
    std::lock_guard<std::mutex> lk(g_mtx);
    return g_status.enabled;
}

}  // namespace office_suite
