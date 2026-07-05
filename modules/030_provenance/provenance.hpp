#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Provenance Tracker: records where a memory entry came from. Distinct
// from the source TEXT field on the entry itself (which is composer-
// rendered shorthand): provenance is the structured audit log that
// answers "where can I jump back to the originating context, and is
// the content still the content we ingested?".
//
// Stored as JSONL at <store_dir>/provenance.jsonl. Hash is FNV-1a 64
// over the recorded content; a content drift on the source file or
// URL is detectable later by recomputing and comparing.
namespace provenance {

enum class Scope { Project = 0, Personal = 1 };

enum class Kind {
    File    = 0,    // ref = absolute path
    Url     = 1,    // ref = http(s) URL
    Chat    = 2,    // ref = "session:<id>:turn:<n>" or similar
    Command = 3,    // ref = shell invocation
    Manual  = 4,    // ref = "ui" / "cli" / free-form
    Merge   = 5,    // ref = list of source ids comma-joined
};
const char * to_string(Kind k);

struct Record {
    int64_t      id           = 0;     // sequential, monotonically increasing
    int64_t      ts           = 0;
    int64_t      entry_id     = 0;     // id in the live store
    Kind         kind         = Kind::Manual;
    std::string  ref;                  // type-specific reference
    std::string  hash;                  // "fnv64:<hex>"
    std::string  excerpt;               // up to 256 chars of the captured content
};

void init();
void shutdown();

// Compute fnv64 over the given bytes and format as "fnv64:<16-hex>".
std::string hash_fnv64(std::string_view content);

// Append a provenance record. Returns the record id.
int64_t track(Scope scope,
              int64_t entry_id,
              Kind kind,
              std::string_view ref,
              std::string_view captured_content,
              std::string_view project_root);

std::vector<Record>          all       (Scope scope, std::string_view project_root);
std::vector<Record>          for_entry (Scope scope, int64_t entry_id,
                                        std::string_view project_root);
std::optional<Record>        get       (Scope scope, int64_t id,
                                        std::string_view project_root);

// Recompute hash for `now_content` and compare against the record's
// stored hash; returns true if they match (i.e. no drift). Returns
// false on a hash mismatch or unknown record.
bool verify(const Record & r, std::string_view now_content);

}
