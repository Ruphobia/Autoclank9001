#include "exposure_calculator.hpp"

namespace exposure_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Exposure Calculator (Photography and cinematography). Awaits wire-up.";
    return s;
}

}
