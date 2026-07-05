#pragma once

#include <string>

// Stub module for the "Condition and Conservation Log" tool entry.
// Numismatics, philately, collecting, antiques.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace condition_and_conservation_log {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
