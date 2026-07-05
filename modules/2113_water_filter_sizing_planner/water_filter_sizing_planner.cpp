#include "water_filter_sizing_planner.hpp"

namespace water_filter_sizing_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Water filter sizing planner (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
