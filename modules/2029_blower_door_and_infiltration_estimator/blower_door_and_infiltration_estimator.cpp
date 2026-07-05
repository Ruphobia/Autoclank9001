#include "blower_door_and_infiltration_estimator.hpp"

namespace blower_door_and_infiltration_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Blower-door and infiltration estimator (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
