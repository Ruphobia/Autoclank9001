#include "demand_response_tou_optimizer.hpp"

namespace demand_response_tou_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Demand Response & TOU Optimizer (Energy systems). Awaits wire-up.";
    return s;
}

}
