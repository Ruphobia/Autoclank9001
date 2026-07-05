#pragma once

#include <string>

// Stub module for the "Landscape fragmentation analyzer" tool entry.
// Ecology, conservation, sustainability.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace landscape_fragmentation_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
