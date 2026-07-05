#include "fuel_range_and_provisioning_calculator.hpp"

namespace fuel_range_and_provisioning_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fuel, range, and provisioning calculator (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
