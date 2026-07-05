#pragma once

#include <string>

// Stub module for the "Service panel load calculator" tool entry.
// Home and DIY (plumbing, residential electrical, HVAC, landscaping).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace service_panel_load_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
