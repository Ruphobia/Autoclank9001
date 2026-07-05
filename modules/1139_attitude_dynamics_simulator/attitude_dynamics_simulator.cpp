#include "attitude_dynamics_simulator.hpp"

namespace attitude_dynamics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Attitude Dynamics Simulator (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
