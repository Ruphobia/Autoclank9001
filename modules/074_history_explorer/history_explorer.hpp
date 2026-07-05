#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "../073_git_workbench/git_workbench.hpp"

// History explorer: blame, file history, and pickaxe browser. Builds
// on top of 073_git_workbench / scripting::execute so the heavy
// lifting stays in the git CLI; this module shapes the output.
namespace history_explorer {

struct BlameEntry {
    std::string sha;         // short SHA of the commit that introduced the line
    std::string author;
    std::string date;        // ISO 8601
    size_t      line_number = 0;
    std::string text;        // line content
};

void init();
void shutdown();

// `git blame --line-porcelain` over the entire file or a single
// line, structured into BlameEntry per source line.
std::vector<BlameEntry> blame(std::string_view repo,
                              std::string_view path,
                              size_t line_only = 0);

// Commits that touched `path`, newest first.
std::vector<git_workbench::Commit> file_history(std::string_view repo,
                                                std::string_view path,
                                                std::string_view range = "");

// Pickaxe: commits whose diff adds or removes the literal `term`.
std::vector<git_workbench::Commit> pickaxe(std::string_view repo,
                                           std::string_view term,
                                           std::string_view range = "");

// Group every commit in `range` by author and return (author, commit_count)
// sorted by count descending.
struct AuthorCount {
    std::string author;
    size_t      count = 0;
};
std::vector<AuthorCount> commits_by_author(std::string_view repo,
                                           std::string_view range = "");

}
