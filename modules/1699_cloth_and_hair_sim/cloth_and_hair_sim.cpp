#include "cloth_and_hair_sim.hpp"

namespace cloth_and_hair_sim {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cloth and Hair Sim (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
