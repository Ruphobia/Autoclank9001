#pragma once

#include <string>

// Stub module for the "Acceptance sampling planner" tool entry.
// Quality, safety, risk, and compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace acceptance_sampling_planner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
