#include "particle_designer.hpp"

namespace particle_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Particle Designer (Game development). Awaits wire-up.";
    return s;
}

}
