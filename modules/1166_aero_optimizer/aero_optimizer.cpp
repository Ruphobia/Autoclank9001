#include "aero_optimizer.hpp"

namespace aero_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Aero Optimizer (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
