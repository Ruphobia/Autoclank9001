#include "git_workbench.hpp"

#include "../011_scripting/scripting.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <sstream>

namespace git_workbench {
namespace {

std::string shell_quote(std::string_view s) {
    std::string out;
    out.push_back('\'');
    for (char c : s) {
        if (c == '\'') out.append("'\\''");
        else           out.push_back(c);
    }
    out.push_back('\'');
    return out;
}

RunResult run_git(std::string_view repo, std::string_view subcommand,
                  int timeout_seconds = 30) {
    std::string cmd = "git -C ";
    cmd += shell_quote(repo);
    cmd += " ";
    cmd.append(subcommand.data(), subcommand.size());
    auto e = scripting::execute(cmd, {}, timeout_seconds);
    RunResult r;
    r.exit_code   = e.exit_code;
    r.stdout_text = e.stdout_text;
    r.stderr_text = e.stderr_text;
    return r;
}

std::vector<std::string> split_lines(std::string_view s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == '\n') { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

std::string trim_trailing_nl(std::string s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
    return s;
}

}

void init()     {}
void shutdown() {}

bool git_available() {
    return std::system("command -v git >/dev/null 2>&1") == 0;
}

std::optional<std::string> repo_root(std::string_view path) {
    std::string cmd = "git -C ";
    cmd += shell_quote(path);
    cmd += " rev-parse --show-toplevel";
    auto e = scripting::execute(cmd, {}, 10);
    if (e.exit_code != 0) return std::nullopt;
    return trim_trailing_nl(e.stdout_text);
}

Status status(std::string_view repo) {
    Status s;
    auto r = run_git(repo, "status --porcelain=v1 --branch");
    if (r.exit_code != 0) return s;
    for (const auto & line : split_lines(r.stdout_text)) {
        if (line.empty()) continue;
        if (line.rfind("##", 0) == 0) {
            // "## main...origin/main [ahead 1, behind 2]"
            std::string body = line.substr(2);
            // branch name is up to first space or "..."
            size_t dots = body.find("...");
            std::string head = (dots == std::string::npos)
                ? body : body.substr(0, dots);
            // strip leading whitespace
            while (!head.empty() && std::isspace(static_cast<unsigned char>(head.front()))) head.erase(0, 1);
            s.branch = head;
            // ahead / behind
            auto lb = body.find('[');
            if (lb != std::string::npos) {
                auto rb = body.find(']', lb);
                std::string inside = body.substr(lb + 1, rb - lb - 1);
                std::sscanf(inside.c_str(), "ahead %d", &s.ahead);
                auto bp = inside.find("behind");
                if (bp != std::string::npos)
                    std::sscanf(inside.c_str() + bp, "behind %d", &s.behind);
            }
            continue;
        }
        if (line.size() < 3) continue;
        std::string code = line.substr(0, 2);
        std::string path = line.substr(3);
        if (code == "??") { s.untracked.push_back(path); continue; }
        if (code[0] != ' ' && code[0] != '?') {
            StatusEntry e; e.path = path; e.status_code = code; s.staged.push_back(e);
        }
        if (code[1] != ' ' && code[1] != '?') {
            StatusEntry e; e.path = path; e.status_code = code; s.unstaged.push_back(e);
        }
    }
    return s;
}

std::string current_branch(std::string_view repo) {
    auto r = run_git(repo, "rev-parse --abbrev-ref HEAD");
    if (r.exit_code != 0) return {};
    return trim_trailing_nl(r.stdout_text);
}

std::string diff(std::string_view repo, bool staged, std::string_view path) {
    std::string cmd = "diff";
    if (staged) cmd += " --staged";
    if (!path.empty()) { cmd += " -- "; cmd += shell_quote(path); }
    auto r = run_git(repo, cmd);
    return r.stdout_text;
}

std::vector<Commit> log(std::string_view repo, std::string_view range) {
    std::vector<Commit> out;
    std::string cmd = "log --no-color --pretty=format:%h%x09%an%x09%aI%x09%s ";
    cmd.append(range.data(), range.size());
    auto r = run_git(repo, cmd);
    if (r.exit_code != 0) return out;
    for (const auto & line : split_lines(r.stdout_text)) {
        if (line.empty()) continue;
        // sha \t author \t date \t subject
        size_t t1 = line.find('\t');
        if (t1 == std::string::npos) continue;
        size_t t2 = line.find('\t', t1 + 1);
        if (t2 == std::string::npos) continue;
        size_t t3 = line.find('\t', t2 + 1);
        if (t3 == std::string::npos) continue;
        Commit c;
        c.sha     = line.substr(0, t1);
        c.author  = line.substr(t1 + 1, t2 - t1 - 1);
        c.date    = line.substr(t2 + 1, t3 - t2 - 1);
        c.subject = line.substr(t3 + 1);
        out.push_back(c);
    }
    return out;
}

std::vector<Branch> branches(std::string_view repo, bool include_remote) {
    std::vector<Branch> out;
    std::string cmd = include_remote ? "branch -a --no-color" : "branch --no-color";
    auto r = run_git(repo, cmd);
    if (r.exit_code != 0) return out;
    for (const auto & line : split_lines(r.stdout_text)) {
        if (line.empty()) continue;
        Branch b;
        b.current = !line.empty() && line[0] == '*';
        std::string name = (line.size() > 2) ? line.substr(2) : line;
        // skip "(HEAD detached at ...)" entries
        if (name.rfind("(HEAD", 0) == 0) continue;
        b.remote  = name.rfind("remotes/", 0) == 0;
        if (b.remote) name = name.substr(8);
        b.name = name;
        out.push_back(b);
    }
    return out;
}

RunResult create_branch(std::string_view repo, std::string_view name) {
    std::string cmd = "branch ";
    cmd += shell_quote(name);
    return run_git(repo, cmd);
}
RunResult checkout(std::string_view repo, std::string_view ref) {
    std::string cmd = "checkout ";
    cmd += shell_quote(ref);
    return run_git(repo, cmd);
}
RunResult stage(std::string_view repo, const std::vector<std::string> & paths) {
    std::string cmd = "add --";
    for (const auto & p : paths) { cmd += " "; cmd += shell_quote(p); }
    return run_git(repo, cmd);
}
RunResult unstage(std::string_view repo, const std::vector<std::string> & paths) {
    std::string cmd = "restore --staged --";
    for (const auto & p : paths) { cmd += " "; cmd += shell_quote(p); }
    return run_git(repo, cmd);
}
RunResult commit_msg(std::string_view repo, std::string_view message) {
    std::string cmd = "commit --no-gpg-sign -m ";
    cmd += shell_quote(message);
    return run_git(repo, cmd);
}
RunResult stash_push(std::string_view repo, std::string_view message) {
    std::string cmd = "stash push -m ";
    cmd += shell_quote(message);
    return run_git(repo, cmd);
}
RunResult stash_pop(std::string_view repo) {
    return run_git(repo, "stash pop");
}
std::vector<std::string> stash_list(std::string_view repo) {
    auto r = run_git(repo, "stash list --no-color");
    return split_lines(r.stdout_text);
}

}
