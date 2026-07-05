#pragma once

#include <string>

// Stub module for the "Fire weather index suite" tool entry.
// Climate, weather, and atmospheric science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fire_weather_index_suite {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
