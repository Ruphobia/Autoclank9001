#pragma once

#include <string>

// Stub module for the "CIPW Norm Calculator" tool entry.
// Geology, GIS, and earth sciences.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace cipw_norm_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
