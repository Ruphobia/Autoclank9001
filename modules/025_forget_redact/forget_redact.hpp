#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Forget and Redact: lifecycle operations over the memory stores from
// 022_project_memory and 023_personal_memory. Three flavors:
//   - soft_forget: pulls the live entry, writes it to a JSON tombstone
//     under <store_dir>/tombstones/, then deletes it from the live
//     store. Recoverable via restore().
//   - hard_delete: removes the live entry AND any matching tombstone.
//   - redact: rewrites a regex span in an existing entry's content
//     in place (does not delete; useful when one phrase needs to
//     disappear but the rest of the note is still load-bearing).
//
// Every operation appends a one-line JSON record to
// <store_dir>/audit.jsonl so deletions are traceable.
namespace forget_redact {

enum class Scope { Project = 0, Personal = 1 };
const char * to_string(Scope s);

struct Tombstone {
    int64_t      tombstone_id  = 0;     // sequential within the dir
    int64_t      original_id   = 0;     // id the entry had in the live store
    int64_t      tombstoned_at = 0;     // unix seconds
    std::string  reason;                // free-form
    std::string  category;
    std::string  content;
    std::vector<std::string> tags;
    std::string  source;
};

// project_root is required for Project scope; ignored for Personal.
// trash_window_seconds bounds restore(); older tombstones are purged
// on next call.
void soft_forget   (Scope scope, int64_t id,
                    std::string_view reason,
                    std::string_view project_root,
                    int64_t trash_window_seconds = 7 * 24 * 60 * 60);

void hard_delete   (Scope scope, int64_t id,
                    std::string_view project_root);

// Replace every regex match in the entry's content with `replacement`
// (default "[REDACTED]"). Updates in place. Returns the number of
// substitutions performed.
size_t redact      (Scope scope, int64_t id,
                    std::string_view pattern,
                    std::string_view project_root,
                    std::string_view replacement = "[REDACTED]");

// Discard tombstones older than `older_than_unix`. Returns count purged.
size_t purge_older (Scope scope, int64_t older_than_unix,
                    std::string_view project_root);

// Tombstone listings + restoration.
std::vector<Tombstone>      list_tombstones(Scope scope, std::string_view project_root);
std::optional<Tombstone>    read_tombstone (Scope scope, int64_t tombstone_id,
                                            std::string_view project_root);

// Restore a tombstoned entry into the live store. Returns the live id
// it received (the original id is not preserved). The tombstone JSON
// is removed on success.
int64_t restore(Scope scope, int64_t tombstone_id,
                std::string_view project_root);

}
