#pragma once

#include <string>

// Stub module for the "RANS Solver" tool entry.
// Aerodynamics and CFD.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace rans_solver {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
