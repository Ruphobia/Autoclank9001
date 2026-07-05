#include "hvac_load_calculator.hpp"

namespace hvac_load_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HVAC Load Calculator (Civil, architectural, and HVAC engineering). Awaits wire-up.";
    return s;
}

}
