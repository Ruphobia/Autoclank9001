#pragma once

#include <string>

// Stub module for the "Wing and Fuselage Drag Estimator" tool entry.
// Aerodynamics and CFD.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace wing_and_fuselage_drag_estimator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
