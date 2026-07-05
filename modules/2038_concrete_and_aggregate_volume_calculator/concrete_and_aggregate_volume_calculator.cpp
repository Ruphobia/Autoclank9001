#include "concrete_and_aggregate_volume_calculator.hpp"

namespace concrete_and_aggregate_volume_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Concrete and aggregate volume calculator (Home and DIY (plumbing, residential electrical, HVAC, landscaping)). Awaits wire-up.";
    return s;
}

}
