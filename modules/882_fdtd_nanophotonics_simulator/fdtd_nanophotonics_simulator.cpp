#include "fdtd_nanophotonics_simulator.hpp"

namespace fdtd_nanophotonics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FDTD Nanophotonics Simulator (Optics and photonics). Awaits wire-up.";
    return s;
}

}
