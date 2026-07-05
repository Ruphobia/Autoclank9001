#include "exposure_time_calculator.hpp"

namespace exposure_time_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Exposure Time Calculator (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
