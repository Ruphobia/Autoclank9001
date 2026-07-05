#include "orbital_simulator.hpp"

namespace orbital_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Orbital Simulator (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
