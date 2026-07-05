#pragma once

#include <string>

// Stub module for the "Interpretation pipeline inspector" tool entry.
// Data, statistics, and ML.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace interpretation_pipeline_inspector {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
