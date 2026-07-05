#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace classify {

struct Result {
    std::string              act;        // command|question|statement|acknowledgment|correction
    std::string              subtype;    // depends on act (see classify.cpp examples)
    std::vector<std::string> tags;       // optional flags (destructive, needs-web, persistent, ...)
    std::string              rationale;  // one short sentence
};

void init();

// Single-call classifier. Operates on the cleaned input (rewrite optional;
// pass empty when classifying early in the pipeline).
Result analyze(std::string_view cleaned, std::string_view rewrite = {});

// Compact tag block formatter, e.g. "[act=command subtype=shell tags=destructive]".
// Used by main.cpp to label every turn's output.
std::string format_tags(const Result & r);

}
