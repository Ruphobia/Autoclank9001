#include "air_quality_aggregator.hpp"

namespace air_quality_aggregator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Air quality aggregator (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
