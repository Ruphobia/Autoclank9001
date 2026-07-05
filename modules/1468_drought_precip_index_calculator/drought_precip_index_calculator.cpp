#include "drought_precip_index_calculator.hpp"

namespace drought_precip_index_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Drought & precip index calculator (Climate, weather, and atmospheric science). Awaits wire-up.";
    return s;
}

}
