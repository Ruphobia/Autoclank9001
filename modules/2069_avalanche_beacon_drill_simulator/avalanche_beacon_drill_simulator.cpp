#include "avalanche_beacon_drill_simulator.hpp"

namespace avalanche_beacon_drill_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Avalanche Beacon Drill Simulator (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
