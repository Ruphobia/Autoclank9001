#include "equilibrium_cure_brine_calculator.hpp"

namespace equilibrium_cure_brine_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Equilibrium Cure & Brine Calculator (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
