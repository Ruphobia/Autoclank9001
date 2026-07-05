#pragma once

#include <string>

// Stub module for the "Gas line sizer" tool entry.
// Home and DIY (plumbing, residential electrical, HVAC, landscaping).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace gas_line_sizer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
