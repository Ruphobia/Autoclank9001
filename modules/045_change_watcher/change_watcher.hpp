#pragma once

#include <string>
#include <string_view>
#include <vector>

// Change Watcher: rendered-text-level diff between two fetches of the
// same URL, with regex noise masks that neutralize rotating content
// (CSRF tokens, request ids, timestamps, ad slots) before the diff so
// they do not register as "real" content changes.
//
// Diff output is a per-line Added / Removed / Same trace plus a
// similarity score in [0, 1] (1 = no meaningful change).
namespace change_watcher {

struct NoiseMask {
    std::string pattern;       // ECMAScript regex
    std::string replacement;   // string that overwrites every match
};

struct LineDiff {
    enum class Kind { Same = 0, Added = 1, Removed = 2 };
    Kind        kind;
    std::string text;
};

struct ChangeReport {
    bool                  changed    = false;
    double                similarity = 1.0;   // 1 = identical after masking
    std::vector<LineDiff> diff;
    std::string           masked_before;
    std::string           masked_after;
};

void init();
void shutdown();

// Apply every mask to `body` in order; returns the masked body.
std::string apply_masks(std::string_view body, const std::vector<NoiseMask> & masks);

// Compare two bodies after applying the same mask list to both. The
// diff is a simple line-by-line longest-common-prefix / suffix model;
// good enough for the "did this page change" probe.
ChangeReport compare(std::string_view before,
                     std::string_view after,
                     const std::vector<NoiseMask> & masks = {});

}
