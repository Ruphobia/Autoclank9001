#pragma once

#include <string>

// Stub module for the "Radio link planner" tool entry.
// Survival, emergency preparedness, off-grid.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace radio_link_planner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
