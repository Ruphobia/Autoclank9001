#pragma once

#include <string>
#include <string_view>
#include <vector>

// Changelog and release tool: groups commit messages by conventional-
// commit type, suggests a semver bump from the set of types and any
// "BREAKING CHANGE" markers, and renders the whole thing as
// markdown. read_git_log() is a thin wrapper around `git log` for
// real use; the rendering layer is pure data so tests do not need a
// repo.
namespace changelog {

struct Commit {
    std::string sha;        // short SHA is fine
    std::string subject;    // commit subject line
    std::string body;       // optional commit body (BREAKING CHANGE etc.)
};

enum class SemverBump { Patch = 0, Minor = 1, Major = 2 };
const char * to_string(SemverBump);

struct Group {
    std::string              label;        // human-facing, e.g. "Features"
    std::string              ccm_type;     // conventional-commit type key
    std::vector<Commit>      commits;
};

void init();
void shutdown();

// Parse the commit subject for `type(scope): summary` conventional-commit
// shape. Returns the lowercased type (empty when no recognizable prefix).
std::string detect_type(std::string_view subject);

// Group commits by conventional type. Unknown types land in a single
// "Other" group at the end.
std::vector<Group> group_by_type(const std::vector<Commit> & commits);

// Pick a semver bump from a commit set: Major when any commit has a
// "BREAKING CHANGE" marker in the body or a "!" after the type; Minor
// when at least one "feat" appears; Patch otherwise.
SemverBump suggest_bump(const std::vector<Commit> & commits);

// Render the changelog as Markdown with one ## section per group.
// `title` is the header line (e.g. "## v1.2.3 - 2026-06-29").
std::string render_markdown(const std::vector<Commit> & commits,
                            std::string_view title);

// Shell out to `git log <range> --pretty=format:%H%x09%s%x00%b%x1e` and
// parse into Commits. Returns an empty vector when the command fails.
// `range` is anything git accepts (e.g. "v1.0.0..HEAD" or just "-20").
std::vector<Commit> read_git_log(std::string_view repo_path,
                                 std::string_view range);

}
