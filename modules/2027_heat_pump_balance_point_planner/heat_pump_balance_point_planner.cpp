#include "heat_pump_balance_point_planner.hpp"

namespace heat_pump_balance_point_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heat pump balance point planner (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
