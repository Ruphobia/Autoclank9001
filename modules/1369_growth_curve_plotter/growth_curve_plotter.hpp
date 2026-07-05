#pragma once

#include <string>

// Stub module for the "Growth curve plotter" tool entry.
// Microbiology, virology, and immunology.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace growth_curve_plotter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
