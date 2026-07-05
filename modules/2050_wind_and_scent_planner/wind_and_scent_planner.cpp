#include "wind_and_scent_planner.hpp"

namespace wind_and_scent_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wind and Scent Planner (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
