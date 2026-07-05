#include "fiber_mode_solver.hpp"

namespace fiber_mode_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fiber Mode Solver (Optics and photonics). Awaits wire-up.";
    return s;
}

}
