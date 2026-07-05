#pragma once

#include <string>

// Stub module for the "Sun and Shade Analyzer" tool entry.
// Agriculture and horticulture.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sun_and_shade_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
