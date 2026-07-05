#include "gpx_track_workbench.hpp"

namespace gpx_track_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GPX Track Workbench (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
