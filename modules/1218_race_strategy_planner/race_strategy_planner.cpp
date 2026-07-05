#include "race_strategy_planner.hpp"

namespace race_strategy_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Race Strategy Planner (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
