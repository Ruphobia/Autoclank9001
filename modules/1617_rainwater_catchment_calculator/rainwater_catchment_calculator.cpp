#include "rainwater_catchment_calculator.hpp"

namespace rainwater_catchment_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rainwater Catchment Calculator (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
