#pragma once

#include <string>

// Stub module for the "ESC, motor, prop matching" tool entry.
// Radio-control modeling (planes, cars, trucks, boats, drones).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace esc_motor_prop_matching {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
