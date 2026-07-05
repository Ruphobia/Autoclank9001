#include "circuit_physics_sim.hpp"

namespace circuit_physics_sim {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Circuit physics sim (Physics and math). Awaits wire-up.";
    return s;
}

}
