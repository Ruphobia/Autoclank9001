#pragma once

#include <string>

// Stub module for the "Decay heat estimator" tool entry.
// Heavy engineering: naval, petroleum, mining, nuclear.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace decay_heat_estimator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
