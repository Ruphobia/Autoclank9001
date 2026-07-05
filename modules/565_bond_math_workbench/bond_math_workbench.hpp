#pragma once

#include <string>

// Stub module for the "Bond Math Workbench" tool entry.
// Finance, markets, accounting.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace bond_math_workbench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
