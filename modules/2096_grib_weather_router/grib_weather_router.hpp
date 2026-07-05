#pragma once

#include <string>

// Stub module for the "GRIB weather router" tool entry.
// Sailing, boating, navigation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace grib_weather_router {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
