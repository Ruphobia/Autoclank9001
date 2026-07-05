#pragma once

#include <string>

// Stub module for the "Field Dressing and Yield Planner" tool entry.
// Hunting, fishing, trapping, wildlife tracking.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace field_dressing_and_yield_planner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
