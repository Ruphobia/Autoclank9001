#include "battery_and_power_budget_planner.hpp"

namespace battery_and_power_budget_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Battery and power-budget planner (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
