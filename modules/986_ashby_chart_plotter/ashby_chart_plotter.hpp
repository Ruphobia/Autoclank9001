#pragma once

#include <string>

// Stub module for the "Ashby Chart Plotter" tool entry.
// Materials science and metallurgy.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace ashby_chart_plotter {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
