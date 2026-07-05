#pragma once

#include <string>

// Stub module for the "Ring and Chain Sizer" tool entry.
// Traditional crafts: horology, blacksmithing, jewelry, glassblowing, pottery, leatherwork, cobbling.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ring_and_chain_sizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
