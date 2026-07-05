#include "greenhouse_climate_simulator.hpp"

namespace greenhouse_climate_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Greenhouse Climate Simulator (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
