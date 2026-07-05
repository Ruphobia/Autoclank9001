#pragma once

#include <string>

// Stub module for the "Heat-transfer solver" tool entry.
// Physics and math.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace heat_transfer_solver {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
