#include "rocket_equation_solver.hpp"

namespace rocket_equation_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rocket Equation Solver (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
