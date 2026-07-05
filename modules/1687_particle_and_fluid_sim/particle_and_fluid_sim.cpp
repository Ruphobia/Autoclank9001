#include "particle_and_fluid_sim.hpp"

namespace particle_and_fluid_sim {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Particle and Fluid Sim (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
