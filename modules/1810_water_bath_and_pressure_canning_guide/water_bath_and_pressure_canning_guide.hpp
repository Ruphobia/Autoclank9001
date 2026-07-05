#pragma once

#include <string>

// Stub module for the "Water-Bath and Pressure Canning Guide" tool entry.
// Cooking, brewing, fermentation, food science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace water_bath_and_pressure_canning_guide {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
