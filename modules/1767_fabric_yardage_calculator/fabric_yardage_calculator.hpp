#pragma once

#include <string>

// Stub module for the "Fabric Yardage Calculator" tool entry.
// Textiles, sewing, soft crafts.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fabric_yardage_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
