#pragma once

#include <string>

// Stub module for the "Composition Overlay" tool entry.
// Photography and cinematography.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace composition_overlay {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
