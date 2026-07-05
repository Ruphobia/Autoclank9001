#pragma once

#include <string>

// Stub module for the "Harmonic Analyzer" tool entry.
// Music composition and theory.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace harmonic_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
