#include "carbon_footprint_calculator.hpp"

namespace carbon_footprint_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Carbon footprint calculator (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
