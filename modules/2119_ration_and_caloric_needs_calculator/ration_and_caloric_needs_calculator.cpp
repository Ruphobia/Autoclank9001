#include "ration_and_caloric_needs_calculator.hpp"

namespace ration_and_caloric_needs_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ration and caloric needs calculator (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
