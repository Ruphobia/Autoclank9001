#include "physics_simulator.hpp"

namespace physics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Physics simulator (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
