#include "grib_weather_router.hpp"

namespace grib_weather_router {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GRIB weather router (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
