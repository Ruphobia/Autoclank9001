#include "quantity_takeoff_cost_estimator.hpp"

namespace quantity_takeoff_cost_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Quantity Takeoff & Cost Estimator (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
