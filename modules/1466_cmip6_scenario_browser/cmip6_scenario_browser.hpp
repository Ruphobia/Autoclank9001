#pragma once

#include <string>

// Stub module for the "CMIP6 scenario browser" tool entry.
// Climate, weather, and atmospheric science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace cmip6_scenario_browser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
