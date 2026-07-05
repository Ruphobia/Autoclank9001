#include "adaptive_optics_simulator.hpp"

namespace adaptive_optics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Adaptive Optics Simulator (Optics and photonics). Awaits wire-up.";
    return s;
}

}
