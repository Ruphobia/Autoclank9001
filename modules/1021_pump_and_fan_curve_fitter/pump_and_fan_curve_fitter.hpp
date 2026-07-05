#pragma once

#include <string>

// Stub module for the "Pump and fan curve fitter" tool entry.
// Acoustic, thermal, hydraulic, pneumatic engineering.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pump_and_fan_curve_fitter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
