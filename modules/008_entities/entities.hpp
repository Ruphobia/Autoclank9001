#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace entities {

struct Mention {
    std::string original;   // exact substring as it appeared
    std::string canonical;  // canonical form per model world knowledge
    std::string summary;    // one-sentence description
};

// Idempotent; readies the underlying 14B model.
void init();

// Extract named entity mentions from `text` and canonicalize each to its
// public-knowledge form. Skips any mention the model is not confident
// about. Returns empty vector when no resolvable entities are found.
//
// Costs one 14B inference call (~200-500 output tokens). Use sparingly --
// once per user input/answer is reasonable.
std::vector<Mention> extract(std::string_view text);

}
