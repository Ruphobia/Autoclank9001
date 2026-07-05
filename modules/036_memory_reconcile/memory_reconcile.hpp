#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>
#include <vector>

// Memory Diff and Reconcile: compare snapshots of the memory store
// across machines or branches. Local ids are not stable across
// machines, so identity is computed instead from a content-derived
// fingerprint over (category, content, sorted tags). A 2-way diff
// surfaces left-only / right-only / common; a 3-way classify
// (base, left, right) labels each record as unchanged, added on one
// side, added on both, or removed on one side. Conflict resolution
// is left to the caller; this module is the data-shape layer that
// makes it possible.
namespace memory_reconcile {

struct Record {
    int64_t                  ts = 0;
    std::string              category;
    std::string              content;
    std::vector<std::string> tags;
    std::string              source;
    std::string              fingerprint;        // computed; stable across machines
};

// Map from fingerprint to Record so callers can union / intersect by
// content identity rather than local id.
using Snapshot = std::map<std::string, Record>;

void init();
void shutdown();

// Compute the fingerprint for a (category, content, tags) tuple.
// tags are sorted internally so call order does not change the hash.
std::string fingerprint(std::string_view category,
                        std::string_view content,
                        std::vector<std::string> tags);

// Read a snapshot from a JSONL file written by 035_export_import.
// Malformed lines are silently dropped (per import convention).
Snapshot load_jsonl(std::string_view src_path);

struct TwoWayDiff {
    std::vector<Record> left_only;
    std::vector<Record> right_only;
    std::vector<Record> common;
};
TwoWayDiff diff(const Snapshot & left, const Snapshot & right);

struct ThreeWayClassification {
    std::vector<Record> unchanged;        // present in base, left, right
    std::vector<Record> added_left;       // in left, not in base or right
    std::vector<Record> added_right;      // in right, not in base or left
    std::vector<Record> added_both;       // in left and right but not base (parallel ingest)
    std::vector<Record> removed_left;     // in base and right, missing in left
    std::vector<Record> removed_right;    // in base and left, missing in right
};
ThreeWayClassification classify(const Snapshot & base,
                                const Snapshot & left,
                                const Snapshot & right);

}
