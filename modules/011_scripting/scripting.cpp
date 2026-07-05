// SPDX-License-Identifier: GPL-3.0-or-later
#include "scripting.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <regex>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <nlohmann/json.hpp>

// Forward-declare coder so the runtime piece does not pull in the model
// header (and its llama.cpp deps) unless someone actually calls draft().
// Must match coder.hpp exactly (including the truncation out-param).
namespace coder {
std::string generate(std::string_view system_prompt,
                     std::string_view user_msg,
                     int              max_new_tokens,
                     bool *           truncated = nullptr);
}

namespace scripting {
namespace {

namespace fs = std::filesystem;

std::string sanitize_name(std::string_view raw) {
    std::string out;
    out.reserve(raw.size());
    for (char c : raw) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '_' || c == '-') {
            out.push_back(c);
        }
    }
    return out;
}

fs::path scripts_dir(std::string_view project_root) {
    fs::path root = project_root.empty()
                        ? fs::current_path()
                        : fs::path(std::string(project_root));
    return root / ".tool" / "scripts";
}

bool unterminated_quotes(std::string_view body) {
    bool in_single = false, in_double = false, in_escape = false;
    for (char c : body) {
        if (in_escape) { in_escape = false; continue; }
        if (c == '\\' && !in_single) { in_escape = true; continue; }
        if (c == '\'' && !in_double) in_single = !in_single;
        else if (c == '"' && !in_single) in_double = !in_double;
    }
    return in_single || in_double;
}

}

void init()     { /* no model load; persistence dir is created lazily on save */ }
void shutdown() { /* nothing held */ }

std::vector<std::string> validate(std::string_view body) {
    std::vector<std::string> warnings;

    bool only_ws = true;
    for (char c : body) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') { only_ws = false; break; }
    }
    if (only_ws) {
        warnings.emplace_back("script body is empty");
        return warnings;
    }

    if (unterminated_quotes(body)) {
        warnings.emplace_back("unterminated quote in script body");
    }

    // Footguns. These are heuristics, not a sandbox; the user is expected
    // to read the script before running it. The point is to surface the
    // obvious cases so they cannot slip past in a long generated block.
    static const std::pair<std::regex, const char *> kFootguns[] = {
        {std::regex(R"(\brm\s+(-[a-zA-Z]*r[a-zA-Z]*f|-[a-zA-Z]*f[a-zA-Z]*r)\s+/\s*($|[^A-Za-z0-9_./]))"),
         "rm -rf against / detected"},
        {std::regex(R"(\bdd\s+[^|;\n]*of=/dev/(sd|nvme|hd|mmcblk))"),
         "dd writing to a raw block device detected"},
        {std::regex(R"(:\(\)\s*\{\s*:\|:&\s*\};:)"),
         "fork bomb pattern detected"},
        {std::regex(R"(\bmkfs\.[a-z0-9]+\s+/dev/)"),
         "mkfs against a raw device detected"},
        {std::regex(R"(>\s*/dev/(sd|nvme|hd|mmcblk))"),
         "shell redirect to a raw block device detected"},
    };
    std::string s(body);
    for (const auto & [re, msg] : kFootguns) {
        if (std::regex_search(s, re)) warnings.emplace_back(msg);
    }
    return warnings;
}

ExecResult execute(std::string_view body,
                   std::string_view cwd,
                   int              timeout_seconds) {
    ExecResult r;

    int out_pipe[2] = {-1, -1};
    int err_pipe[2] = {-1, -1};
    if (::pipe(out_pipe) != 0 || ::pipe(err_pipe) != 0) {
        r.stderr_text = std::string("pipe failed: ") + std::strerror(errno);
        return r;
    }

    pid_t pid = ::fork();
    if (pid < 0) {
        r.stderr_text = std::string("fork failed: ") + std::strerror(errno);
        for (int fd : {out_pipe[0], out_pipe[1], err_pipe[0], err_pipe[1]})
            if (fd >= 0) ::close(fd);
        return r;
    }

    if (pid == 0) {
        // Child: redirect stdout/stderr to pipe write ends, then exec bash.
        ::dup2(out_pipe[1], STDOUT_FILENO);
        ::dup2(err_pipe[1], STDERR_FILENO);
        ::close(out_pipe[0]); ::close(out_pipe[1]);
        ::close(err_pipe[0]); ::close(err_pipe[1]);

        // Own process group so the parent can SIGTERM the whole script,
        // not just the bash that launched its children.
        ::setpgid(0, 0);

        if (!cwd.empty()) {
            if (::chdir(std::string(cwd).c_str()) != 0) {
                ::dprintf(STDERR_FILENO, "chdir(%.*s) failed: %s\n",
                          (int)cwd.size(), cwd.data(), std::strerror(errno));
                ::_exit(127);
            }
        }

        std::string body_str(body);
        const char * argv[] = {"bash", "-c", body_str.c_str(), nullptr};
        ::execvp("bash", const_cast<char * const *>(argv));
        ::dprintf(STDERR_FILENO, "execvp(bash) failed: %s\n", std::strerror(errno));
        ::_exit(127);
    }

    // Parent.
    ::close(out_pipe[1]); ::close(err_pipe[1]);
    ::setpgid(pid, pid);   // race-safe duplicate of the child's setpgid

    // Non-blocking reads so a slow child doesn't starve the timeout
    // accounting. We poll on both pipes with a short interval and tally
    // wall-clock elapsed time against the timeout.
    ::fcntl(out_pipe[0], F_SETFL, O_NONBLOCK);
    ::fcntl(err_pipe[0], F_SETFL, O_NONBLOCK);

    auto pull = [&](int fd, std::string & sink) {
        char buf[4096];
        for (;;) {
            ssize_t n = ::read(fd, buf, sizeof(buf));
            if (n > 0) { sink.append(buf, (size_t)n); continue; }
            break;
        }
    };

    const long deadline_ms = (timeout_seconds <= 0) ? -1
                                                    : (long)timeout_seconds * 1000;
    long elapsed_ms = 0;
    const long step_ms = 50;
    bool killed = false;

    for (;;) {
        int   status = 0;
        pid_t w      = ::waitpid(pid, &status, WNOHANG);
        pull(out_pipe[0], r.stdout_text);
        pull(err_pipe[0], r.stderr_text);

        if (w == pid) {
            // Drain any remaining buffered output.
            pull(out_pipe[0], r.stdout_text);
            pull(err_pipe[0], r.stderr_text);
            if (WIFEXITED(status))        r.exit_code = WEXITSTATUS(status);
            else if (WIFSIGNALED(status)) r.exit_code = 128 + WTERMSIG(status);
            break;
        }
        if (w < 0) {
            if (errno == EINTR) continue;
            r.stderr_text += std::string("\nwaitpid failed: ") + std::strerror(errno);
            r.exit_code = -1;
            break;
        }

        if (deadline_ms > 0 && elapsed_ms >= deadline_ms && !killed) {
            ::killpg(pid, SIGTERM);
            r.timed_out = true;
            killed      = true;
            // Give bash a second to flush; then SIGKILL.
            timespec ts{1, 0};
            ::nanosleep(&ts, nullptr);
            ::killpg(pid, SIGKILL);
        }

        timespec ts{0, step_ms * 1000 * 1000};
        ::nanosleep(&ts, nullptr);
        elapsed_ms += step_ms;
    }

    ::close(out_pipe[0]);
    ::close(err_pipe[0]);
    return r;
}

void save(const Script & s, std::string_view project_root) {
    std::string name = sanitize_name(s.name);
    if (name.empty()) throw std::runtime_error("scripting::save: empty/invalid script name");

    fs::path dir = scripts_dir(project_root);
    fs::create_directories(dir);

    fs::path body_path = dir / (name + ".sh");
    fs::path meta_path = dir / (name + ".json");

    std::ofstream body_out(body_path, std::ios::binary | std::ios::trunc);
    if (!body_out) throw std::runtime_error("scripting::save: cannot open " + body_path.string());
    body_out.write(s.body.data(), (std::streamsize)s.body.size());
    body_out.close();

    nlohmann::json j;
    j["name"]        = name;
    j["description"] = s.description;
    std::ofstream meta_out(meta_path, std::ios::binary | std::ios::trunc);
    if (!meta_out) throw std::runtime_error("scripting::save: cannot open " + meta_path.string());
    meta_out << j.dump(2);
}

Script load(std::string_view raw_name, std::string_view project_root) {
    std::string name = sanitize_name(raw_name);
    if (name.empty()) throw std::runtime_error("scripting::load: empty/invalid script name");
    fs::path dir       = scripts_dir(project_root);
    fs::path body_path = dir / (name + ".sh");
    fs::path meta_path = dir / (name + ".json");
    if (!fs::exists(body_path))
        throw std::runtime_error("scripting::load: no such script: " + body_path.string());

    std::ifstream body_in(body_path, std::ios::binary);
    std::ostringstream b;
    b << body_in.rdbuf();

    Script s;
    s.name = name;
    s.body = b.str();
    if (fs::exists(meta_path)) {
        try {
            std::ifstream meta_in(meta_path);
            nlohmann::json j;
            meta_in >> j;
            if (j.contains("description") && j["description"].is_string())
                s.description = j["description"].get<std::string>();
        } catch (...) { /* tolerate a missing or malformed sidecar */ }
    }
    return s;
}

std::vector<std::string> list(std::string_view project_root) {
    std::vector<std::string> names;
    fs::path dir = scripts_dir(project_root);
    if (!fs::exists(dir)) return names;
    for (const auto & e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        auto p = e.path();
        if (p.extension() == ".sh") names.push_back(p.stem().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}

namespace {
constexpr const char * kDraftSystemPrompt =
    "You are a bash script author. The user describes work they want done; "
    "you reply with ONE bash script that does it, and nothing else.\n"
    "\n"
    "RULES:\n"
    "- Output the raw script. No markdown fences, no preface, no commentary.\n"
    "- Assume Ubuntu coreutils plus the usual tools (find, grep, sed, awk, "
    "curl, jq, python3, git).\n"
    "- Prefer idempotent forms (mkdir -p, install -D, etc.).\n"
    "- Do not add interactive prompts; the script runs unattended.\n"
    "- Do not invent flags. If something is ambiguous, pick the most "
    "literal interpretation.\n";
}

std::string draft(std::string_view intent) {
    std::string body = coder::generate(kDraftSystemPrompt, intent, 1024);
    // Strip stray markdown fences if the model added them anyway.
    auto strip_fence = [](std::string & s) {
        if (s.size() >= 3 && s.substr(0, 3) == "```") {
            auto nl = s.find('\n');
            if (nl != std::string::npos) s.erase(0, nl + 1);
        }
        if (s.size() >= 3 && s.substr(s.size() - 3) == "```") {
            s.erase(s.size() - 3);
        }
        while (!s.empty() && (s.back() == '\n' || s.back() == ' ')) s.pop_back();
    };
    strip_fence(body);
    return body;
}

}
