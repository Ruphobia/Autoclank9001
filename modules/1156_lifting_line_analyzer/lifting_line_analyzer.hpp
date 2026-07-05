#pragma once

#include <string>

// Stub module for the "Lifting Line Analyzer" tool entry.
// Aerodynamics and CFD.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace lifting_line_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
