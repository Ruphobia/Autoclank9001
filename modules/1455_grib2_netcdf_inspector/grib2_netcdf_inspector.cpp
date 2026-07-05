#include "grib2_netcdf_inspector.hpp"

namespace grib2_netcdf_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GRIB2/NetCDF inspector (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
