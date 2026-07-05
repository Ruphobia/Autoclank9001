#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Diff viewer: line-level diff with LCS backbone, inline + side-by-
// side rendering, three-way merge with conflict markers, and a small
// stats roll-up. Pure-string surface; no file I/O, no editor widget.
namespace diff_viewer {

struct Line {
    enum class Kind { Same = 0, Removed = 1, Added = 2 };
    Kind         kind;
    std::string  text;
    size_t       before_line = 0;     // 1-based; 0 when this line is Added
    size_t       after_line  = 0;     // 1-based; 0 when this line is Removed
};

struct Diff {
    std::vector<Line> lines;
};

struct Stats {
    size_t same     = 0;
    size_t added    = 0;
    size_t removed  = 0;
};

struct MergeResult {
    std::string text;       // the merged content; conflicts marked with the
                            // standard "<<<<<<<", "=======", ">>>>>>>" fences.
    size_t      conflict_count = 0;
};

void init();
void shutdown();

// Compute a Same/Added/Removed line trace via the textbook LCS.
Diff diff_text(std::string_view before, std::string_view after);

// Inline rendering: "  same line\n- removed line\n+ added line\n".
std::string render_inline(const Diff & d);

// Two-column side-by-side rendering. `column_width` is the per-column
// budget; long lines are truncated with a "..." suffix.
std::string render_side_by_side(const Diff & d, size_t column_width = 60);

// Roll-up counts.
Stats stats(const Diff & d);

// Three-way merge of `left` and `right` against the common `base`.
// Trivial outcomes (one side unchanged) accept the changed side; the
// remaining conflict regions are wrapped in the standard fences.
MergeResult three_way_merge(std::string_view base,
                            std::string_view left,
                            std::string_view right,
                            std::string_view left_label  = "LEFT",
                            std::string_view right_label = "RIGHT");

}
