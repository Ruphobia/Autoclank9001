#include "rng_manipulator.hpp"

namespace rng_manipulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RNG Manipulator (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
