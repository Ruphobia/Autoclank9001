#include "lagrangian_drift_simulator.hpp"

namespace lagrangian_drift_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lagrangian drift simulator (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
