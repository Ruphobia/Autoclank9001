#include "raster_chart_georeferencer.hpp"

namespace raster_chart_georeferencer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Raster chart georeferencer (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
