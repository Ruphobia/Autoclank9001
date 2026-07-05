#pragma once

#include <string>

// Stub module for the "Storm chase nowcaster" tool entry.
// Climate, weather, and atmospheric science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace storm_chase_nowcaster {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
