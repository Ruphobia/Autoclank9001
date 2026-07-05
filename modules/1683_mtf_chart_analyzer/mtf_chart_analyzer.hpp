#pragma once

#include <string>

// Stub module for the "MTF Chart Analyzer" tool entry.
// Photography and cinematography.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace mtf_chart_analyzer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
