#pragma once

#include <string>

// Stub module for the "Props Inventory" tool entry.
// Magic, juggling, stage illusion, performance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace props_inventory {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
