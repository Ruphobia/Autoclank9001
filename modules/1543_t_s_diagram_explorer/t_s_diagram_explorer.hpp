#pragma once

#include <string>

// Stub module for the "T-S diagram explorer" tool entry.
// Oceanography and marine science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace t_s_diagram_explorer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
