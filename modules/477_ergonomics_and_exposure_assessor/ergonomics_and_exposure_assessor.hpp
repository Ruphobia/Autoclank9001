#pragma once

#include <string>

// Stub module for the "Ergonomics and exposure assessor" tool entry.
// Quality, safety, risk, and compliance.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ergonomics_and_exposure_assessor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
