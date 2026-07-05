#include "wind_turbine_power_curve_studio.hpp"

namespace wind_turbine_power_curve_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wind Turbine Power Curve Studio (Energy systems). Awaits wire-up.";
    return s;
}

}
