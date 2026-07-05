#include "polar_solver.hpp"

namespace polar_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Polar Solver (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
