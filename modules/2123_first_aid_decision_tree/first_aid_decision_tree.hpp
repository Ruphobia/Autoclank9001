#pragma once

#include <string>

// Stub module for the "First-aid decision tree" tool entry.
// Survival, emergency preparedness, off-grid.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace first_aid_decision_tree {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
