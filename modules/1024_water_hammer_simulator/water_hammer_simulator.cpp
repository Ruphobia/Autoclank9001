#include "water_hammer_simulator.hpp"

namespace water_hammer_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Water hammer simulator (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}
