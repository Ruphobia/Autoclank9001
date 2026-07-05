#pragma once

#include <string>

// Stub module for the "Phenology and climate tracker" tool entry.
// Ecology, conservation, sustainability.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace phenology_and_climate_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
