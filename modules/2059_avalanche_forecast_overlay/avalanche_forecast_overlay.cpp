#include "avalanche_forecast_overlay.hpp"

namespace avalanche_forecast_overlay {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Avalanche Forecast Overlay (Outdoor recreation: camping, climbing, skiing, paragliding, geocaching). Awaits wire-up.";
    return s;
}

}
