#include "cg_and_balance_calculator.hpp"

namespace cg_and_balance_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CG and balance calculator (Radio-control modeling (planes, cars, trucks, boats, drones)). Awaits wire-up.";
    return s;
}

}
