#include "ode_pde_solver.hpp"

namespace ode_pde_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ODE/PDE solver (Physics and math). Awaits wire-up.";
    return s;
}

}
