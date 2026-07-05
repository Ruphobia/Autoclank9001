#include "rf_coordination_planner.hpp"

namespace rf_coordination_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RF Coordination Planner (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
