#include "lee_carter_mortality_forecaster.hpp"

namespace lee_carter_mortality_forecaster {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lee-Carter mortality forecaster (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
