#pragma once

#include <string>
#include <string_view>

namespace expertise {

// Eagerly readies the underlying 14B model (shared with stylize).
// REQUIRES prompt_cleanup::init() to have run first.
void init();

// Returns a short lowercase label naming the field / area of expertise the
// input is about (e.g. "human biology", "physics", "law", "cooking",
// "general knowledge"). Empty string only on hard error.
std::string classify(std::string_view text);

}
