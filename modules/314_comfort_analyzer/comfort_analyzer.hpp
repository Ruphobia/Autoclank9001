#pragma once

#include <string>

// Stub module for the "Comfort Analyzer" tool entry.
// VR / AR / XR.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace comfort_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
