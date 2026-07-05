#pragma once

#include <string>

// Stub module for the "Fiber Mode Solver" tool entry.
// Optics and photonics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fiber_mode_solver {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
