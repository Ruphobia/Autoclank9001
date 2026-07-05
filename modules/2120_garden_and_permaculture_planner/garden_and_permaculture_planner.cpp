#include "garden_and_permaculture_planner.hpp"

namespace garden_and_permaculture_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Garden and permaculture planner (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
