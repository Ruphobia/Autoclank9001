#include "rans_solver.hpp"

namespace rans_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RANS Solver (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
