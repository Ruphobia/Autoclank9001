#pragma once

#include <string>

// Stub module for the "Fuel, range, and provisioning calculator" tool entry.
// Sailing, boating, navigation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fuel_range_and_provisioning_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
