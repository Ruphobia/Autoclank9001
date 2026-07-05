#pragma once

#include <string>

// Stub module for the "Resource Estimator" tool entry.
// Quantum computing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace resource_estimator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
