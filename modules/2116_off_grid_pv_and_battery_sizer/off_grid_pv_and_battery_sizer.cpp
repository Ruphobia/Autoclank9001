#include "off_grid_pv_and_battery_sizer.hpp"

namespace off_grid_pv_and_battery_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Off-grid PV and battery sizer (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
