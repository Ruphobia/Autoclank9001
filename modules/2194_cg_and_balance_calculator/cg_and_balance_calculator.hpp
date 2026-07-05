#pragma once

#include <string>

// Stub module for the "CG and balance calculator" tool entry.
// Radio-control modeling (planes, cars, trucks, boats, drones).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace cg_and_balance_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
