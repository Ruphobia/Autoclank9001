#include "variational_solver_lab.hpp"

namespace variational_solver_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Variational Solver Lab (Quantum computing). Awaits wire-up.";
    return s;
}

}
