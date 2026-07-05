#pragma once

#include <string>

// Stub module for the "Light Pollution Map" tool entry.
// Astronomy and astrophysics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace light_pollution_map {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
