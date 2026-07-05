#include "haulage_and_fleet_simulator.hpp"

namespace haulage_and_fleet_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Haulage and fleet simulator (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
