#pragma once

#include <string>

// Stub module for the "Propeller series matcher" tool entry.
// Heavy engineering: naval, petroleum, mining, nuclear.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace propeller_series_matcher {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
