#pragma once

#include <string>

// Stub module for the "Heat exchanger sizer" tool entry.
// Acoustic, thermal, hydraulic, pneumatic engineering.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace heat_exchanger_sizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
