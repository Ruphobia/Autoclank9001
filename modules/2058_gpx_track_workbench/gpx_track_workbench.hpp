#pragma once

#include <string>

// Stub module for the "GPX Track Workbench" tool entry.
// Outdoor recreation: camping, climbing, skiing, paragliding, geocaching.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace gpx_track_workbench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
