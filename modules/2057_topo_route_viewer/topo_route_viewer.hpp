#pragma once

#include <string>

// Stub module for the "Topo Route Viewer" tool entry.
// Outdoor recreation: camping, climbing, skiing, paragliding, geocaching.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace topo_route_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
