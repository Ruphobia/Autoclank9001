#include "egress_occupancy_planner.hpp"

namespace egress_occupancy_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Egress & Occupancy Planner (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
