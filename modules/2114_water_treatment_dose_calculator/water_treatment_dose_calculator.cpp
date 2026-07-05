#include "water_treatment_dose_calculator.hpp"

namespace water_treatment_dose_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Water treatment dose calculator (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
