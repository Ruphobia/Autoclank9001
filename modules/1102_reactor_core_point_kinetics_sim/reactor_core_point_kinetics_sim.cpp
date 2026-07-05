#include "reactor_core_point_kinetics_sim.hpp"

namespace reactor_core_point_kinetics_sim {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reactor core point-kinetics sim (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
