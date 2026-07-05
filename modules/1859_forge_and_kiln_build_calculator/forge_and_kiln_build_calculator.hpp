#pragma once

#include <string>

// Stub module for the "Forge and Kiln Build Calculator" tool entry.
// Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace forge_and_kiln_build_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
