#pragma once

#include <string>

// Stub module for the "Wind Turbine Power Curve Studio" tool entry.
// Energy systems.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace wind_turbine_power_curve_studio {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
