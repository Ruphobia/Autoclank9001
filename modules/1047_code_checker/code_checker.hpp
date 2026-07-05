#pragma once

#include <string>

// Stub module for the "Code Checker" tool entry.
// Civil, architectural, and HVAC engineering.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace code_checker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
