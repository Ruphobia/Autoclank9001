#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

// Merge and Deduplicate for project / personal memory entries. Trigram
// Jaccard similarity is the rule-based duplicate detector; an
// embedding-based variant can slot in later without changing the API
// (similarity is just a double). `propose_merge` synthesizes a single
// entry from a group; `apply_merge` writes the merged entry and
// soft-forgets the originals via 025_forget_redact so the operation is
// recoverable.
namespace merge_dedup {

enum class Scope { Project = 0, Personal = 1 };

struct DuplicateGroup {
    std::vector<int64_t> ids;          // entry ids in the live store
    double               similarity = 0.0;   // mean pairwise Jaccard
};

struct MergedDraft {
    std::string              category;
    std::string              content;       // merged body
    std::vector<std::string> tags;          // union of input tags
    std::string              source;        // composer-style synthesized source
    std::vector<int64_t>     merged_from;   // ids that contributed
};

// Detect duplicate clusters in the given scope. Groups are formed by
// transitive closure over pairs whose trigram Jaccard is >= threshold.
// Singletons are not returned. Scans `category` only when non-empty.
std::vector<DuplicateGroup>
    find_duplicates(Scope scope, std::string_view project_root,
                    double threshold = 0.7,
                    std::string_view category = {});

// Synthesize a merged draft. The merged content concatenates unique
// sentences from each input, preserving order of first occurrence.
// Tags are unioned (case-sensitive set); source notes which ids
// contributed.
MergedDraft propose_merge(Scope scope, const std::vector<int64_t> & ids,
                          std::string_view project_root);

// Apply a merge: writes `draft` to the live store, soft-forgets every
// id in `draft.merged_from` via 025_forget_redact (so the merge is
// recoverable). Returns the new live id.
int64_t apply_merge(Scope scope, const MergedDraft & draft,
                    std::string_view project_root);

// Compute trigram Jaccard similarity over two strings.
double trigram_jaccard(std::string_view a, std::string_view b);

}
