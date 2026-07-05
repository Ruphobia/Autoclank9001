#pragma once

#include <string>

// Stub module for the "Lock and deadlock inspector" tool entry.
// Databases.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace lock_and_deadlock_inspector {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
