// SPDX-License-Identifier: GPL-3.0-or-later
#include "hurricane_track_plotter.hpp"

namespace hurricane_track_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hurricane track plotter (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
