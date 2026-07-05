#pragma once

#include <string>

// Stub module for the "Exposure Time Calculator" tool entry.
// Astronomy and astrophysics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace exposure_time_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
