#include "truss_solver.hpp"

namespace truss_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Truss Solver (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
