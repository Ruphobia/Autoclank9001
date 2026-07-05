#include "carbonate_chemistry_calculator.hpp"

namespace carbonate_chemistry_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Carbonate chemistry calculator (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
