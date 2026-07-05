#include "pump_curve_matcher.hpp"

namespace pump_curve_matcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pump curve matcher (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
