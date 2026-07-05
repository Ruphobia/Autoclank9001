#pragma once

#include <string>

// Stub module for the "Bullwhip simulator" tool entry.
// Logistics, supply chain, operations research.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace bullwhip_simulator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
