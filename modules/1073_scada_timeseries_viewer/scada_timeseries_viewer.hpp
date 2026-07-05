#pragma once

#include <string>

// Stub module for the "SCADA timeseries viewer" tool entry.
// Civil utility (water, waste, sanitation).
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace scada_timeseries_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
