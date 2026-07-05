#pragma once

#include <string>
#include <string_view>
#include <vector>

// Footnote and Endnote Manager: utilities for long-document footnote
// hygiene. Works on Pandoc-flavored markdown where references look
// like `[^id]` in body text and definitions look like
// `[^id]: body text` at end-of-document. Renumber by first occurrence,
// detect duplicates, and rewrite consecutive same-citation references
// as "ibid." short forms.
namespace footnote_manager {

struct Footnote {
    std::string id;            // original id as written
    std::string text;          // definition body
    std::vector<size_t> ref_positions;     // byte offsets in body of every [^id]
};

void init();
void shutdown();

// Walk `doc` and pull every [^id] reference and every [^id]: definition.
std::vector<Footnote> extract(std::string_view doc);

// Rewrite doc so footnote ids are renumbered to 1..N in order of first
// reference. Definitions and references are both updated. Returns the
// new document; the order of definitions in the trailer is sorted by
// the new id.
std::string renumber(std::string_view doc);

// In a sequence of [^id] references, replace any reference whose id
// is the same as the immediately previous reference with the literal
// `ibid.` (and drop the [^...]). Returns the new doc text.
std::string apply_ibid(std::string_view doc);

}
