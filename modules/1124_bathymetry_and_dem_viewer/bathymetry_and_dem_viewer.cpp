#include "bathymetry_and_dem_viewer.hpp"

namespace bathymetry_and_dem_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bathymetry and DEM viewer (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
