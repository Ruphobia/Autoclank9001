#pragma once

#include <string>

// Stub module for the "Sail and rig inventory" tool entry.
// Sailing, boating, navigation.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace sail_and_rig_inventory {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
