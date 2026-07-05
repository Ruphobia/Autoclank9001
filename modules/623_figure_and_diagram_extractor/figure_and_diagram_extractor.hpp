#pragma once

#include <string>

// Stub module for the "Figure and diagram extractor" tool entry.
// Documents and reading.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace figure_and_diagram_extractor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
