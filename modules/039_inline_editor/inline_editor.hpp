#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Inline Memory Editor: thin wrapper around 022/023 update() that adds
// diff-aware history and link validation. Every edit snapshots the
// pre-edit state to <store_dir>/edit_history/<id>.jsonl so the user
// can review prior revisions. Body links use the [[123]] convention
// (entry id) and validate_links resolves each one.
namespace inline_editor {

enum class Scope { Project = 0, Personal = 1 };

struct Snapshot {
    int64_t                  revision   = 0;
    int64_t                  ts         = 0;
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;
};

struct LinkRef {
    std::string  raw;          // the [[N]] text as found
    int64_t      target_id;    // parsed N
    bool         exists;       // does target resolve in the same scope
};

struct LineDiff {
    enum class Kind { Same = 0, Removed = 1, Added = 2 };
    Kind         kind;
    std::string  text;
};

void init();
void shutdown();

// Snapshot the current state, then write the new state. Returns the
// revision number that was assigned to the snapshot (i.e. the
// pre-edit body's history index).
int64_t edit(Scope scope, int64_t id,
             std::string_view new_content,
             const std::vector<std::string> & new_tags,
             std::string_view new_source,
             std::string_view project_root);

std::vector<Snapshot> history(Scope scope, int64_t id,
                              std::string_view project_root);

// Compare two revisions; rev=-1 means "current live entry". A simple
// Myers-light line diff is sufficient for the UI overlay.
std::vector<LineDiff> diff_revisions(Scope scope, int64_t id,
                                     int64_t rev_a, int64_t rev_b,
                                     std::string_view project_root);

// Walk `content` for [[<integer>]] references; resolve each against
// the live store in the given scope.
std::vector<LinkRef> validate_links(std::string_view content,
                                    Scope scope,
                                    std::string_view project_root);

}
