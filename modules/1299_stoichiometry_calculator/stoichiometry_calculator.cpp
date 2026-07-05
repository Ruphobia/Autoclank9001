#include "stoichiometry_calculator.hpp"

namespace stoichiometry_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stoichiometry calculator (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
