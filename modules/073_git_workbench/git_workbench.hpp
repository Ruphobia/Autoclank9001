#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Git workbench: thin orchestrator around the `git` CLI for staging,
// committing, branching, and stash management. Each call returns
// structured data parsed from git's machine-readable output. The
// callers (chat, UI, automations) drive higher-level workflows on
// top of this surface.
namespace git_workbench {

struct StatusEntry {
    std::string  path;
    std::string  status_code;     // two-char `git status --porcelain` code
};

struct Status {
    std::vector<StatusEntry> staged;
    std::vector<StatusEntry> unstaged;
    std::vector<std::string> untracked;
    std::string              branch;
    int                      ahead   = 0;
    int                      behind  = 0;
};

struct Commit {
    std::string sha;     // short SHA
    std::string author;
    std::string date;    // ISO 8601
    std::string subject;
};

struct Branch {
    std::string name;
    bool        current = false;
    bool        remote  = false;
};

struct RunResult {
    int          exit_code = -1;
    std::string  stdout_text;
    std::string  stderr_text;
};

void init();
void shutdown();

// True if `git` is on PATH.
bool git_available();

// `git rev-parse --show-toplevel` for a path inside a working tree.
// Returns nullopt when the path is not inside a git repo.
std::optional<std::string> repo_root(std::string_view path);

Status                  status(std::string_view repo);
std::string             current_branch(std::string_view repo);
std::string             diff(std::string_view repo, bool staged = false,
                             std::string_view path = {});
std::vector<Commit>     log(std::string_view repo, std::string_view range = "-20");
std::vector<Branch>     branches(std::string_view repo, bool include_remote = false);

RunResult create_branch(std::string_view repo, std::string_view name);
RunResult checkout      (std::string_view repo, std::string_view ref);
RunResult stage         (std::string_view repo, const std::vector<std::string> & paths);
RunResult unstage       (std::string_view repo, const std::vector<std::string> & paths);
RunResult commit_msg    (std::string_view repo, std::string_view message);
RunResult stash_push    (std::string_view repo, std::string_view message);
RunResult stash_pop     (std::string_view repo);
std::vector<std::string> stash_list(std::string_view repo);

}
