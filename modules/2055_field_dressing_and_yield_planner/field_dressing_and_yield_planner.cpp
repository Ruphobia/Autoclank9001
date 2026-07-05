#include "field_dressing_and_yield_planner.hpp"

namespace field_dressing_and_yield_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Field Dressing and Yield Planner (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
