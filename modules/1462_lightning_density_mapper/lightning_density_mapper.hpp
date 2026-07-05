#pragma once

#include <string>

// Stub module for the "Lightning density mapper" tool entry.
// Climate, weather, and atmospheric science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace lightning_density_mapper {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
