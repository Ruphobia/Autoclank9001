#pragma once

#include <string>

// Stub module for the "Statistical test guide" tool entry.
// Psychology and behavioral science.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace statistical_test_guide {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
