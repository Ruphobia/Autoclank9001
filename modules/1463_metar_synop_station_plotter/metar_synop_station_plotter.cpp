#include "metar_synop_station_plotter.hpp"

namespace metar_synop_station_plotter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: METAR/SYNOP station plotter (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
