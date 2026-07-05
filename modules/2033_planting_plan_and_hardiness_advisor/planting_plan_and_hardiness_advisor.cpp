#include "planting_plan_and_hardiness_advisor.hpp"

namespace planting_plan_and_hardiness_advisor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Planting plan and hardiness advisor (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
