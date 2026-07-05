#include "allometric_dose_calculator.hpp"

namespace allometric_dose_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Allometric dose calculator (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
