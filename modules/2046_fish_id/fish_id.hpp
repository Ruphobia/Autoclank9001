#pragma once

#include <string>

// Stub module for the "Fish ID" tool entry.
// Hunting, fishing, trapping, wildlife tracking.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace fish_id {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
