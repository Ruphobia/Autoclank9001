#include "delta_v_budget_planner.hpp"

namespace delta_v_budget_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Delta-V Budget Planner (Aerospace and orbital mechanics). Awaits wire-up.";
    return s;
}

}
