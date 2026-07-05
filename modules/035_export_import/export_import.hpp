#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Export and Import for the project / personal memory stores.
// Round-trippable to and from two formats:
//   - JSONL (single file, one record per line); the canonical machine
//     format. Sufficient for backup, sync between machines, and
//     migration into other tools.
//   - Markdown bundle (one .md per entry plus an index.json with
//     metadata). The human-readable format; an Obsidian / Logseq /
//     plain-editor user can pick it up directly.
//
// Imports preserve the original timestamps and assign new live ids on
// re-insert. There is intentionally no in-place "merge by id" mode:
// import is additive; reconciliation belongs in a separate module.
namespace export_import {

enum class Scope { Project = 0, Personal = 1 };

struct Counts {
    size_t entries  = 0;
    size_t skipped  = 0;     // malformed records the import refused
};

void init();
void shutdown();

// Write every live entry to a single JSONL at `dest_path`. Returns
// number of records written.
size_t export_jsonl(Scope scope, std::string_view project_root,
                    std::string_view dest_path);

// Write a markdown bundle: `dest_dir/entry-<id>.md` for each entry
// plus `dest_dir/index.json` with the manifest. Returns count.
size_t export_markdown_bundle(Scope scope, std::string_view project_root,
                              std::string_view dest_dir);

// Re-insert every record from a JSONL file into the live store. Each
// record becomes a new entry (new id, original ts preserved when
// possible).
Counts import_jsonl(Scope scope, std::string_view project_root,
                    std::string_view src_path);

// Read a markdown bundle written by export_markdown_bundle and
// re-insert each entry. Uses the bundle's index.json for the metadata.
Counts import_markdown_bundle(Scope scope, std::string_view project_root,
                              std::string_view src_dir);

}
