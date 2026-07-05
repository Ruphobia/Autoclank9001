#pragma once

#include <string>

// Stub module for the "Ray-trace optics bench" tool entry.
// Physics and math.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ray_trace_optics_bench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
