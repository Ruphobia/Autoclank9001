#include "vehicle_dynamics_simulator.hpp"

namespace vehicle_dynamics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vehicle Dynamics Simulator (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
