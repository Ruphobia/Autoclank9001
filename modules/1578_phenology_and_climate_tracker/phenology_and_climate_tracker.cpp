#include "phenology_and_climate_tracker.hpp"

namespace phenology_and_climate_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phenology and climate tracker (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
