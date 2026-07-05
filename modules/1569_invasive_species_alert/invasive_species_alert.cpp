#include "invasive_species_alert.hpp"

namespace invasive_species_alert {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Invasive species alert (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
