#pragma once

#include <string>

// Stub module for the "Amortization schedule builder" tool entry.
// Real estate, insurance, actuarial.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace amortization_schedule_builder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
