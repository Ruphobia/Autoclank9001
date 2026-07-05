#pragma once

#include <string>

// Stub module for the "Eye-tracking analyzer" tool entry.
// Psychology and behavioral science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace eye_tracking_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
