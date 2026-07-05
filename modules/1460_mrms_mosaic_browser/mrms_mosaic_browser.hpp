#pragma once

#include <string>

// Stub module for the "MRMS mosaic browser" tool entry.
// Climate, weather, and atmospheric science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace mrms_mosaic_browser {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
