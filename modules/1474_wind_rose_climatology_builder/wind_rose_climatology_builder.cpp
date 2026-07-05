#include "wind_rose_climatology_builder.hpp"

namespace wind_rose_climatology_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wind rose & climatology builder (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
