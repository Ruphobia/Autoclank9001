#include "equation_and_notation_workbench.hpp"

namespace equation_and_notation_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Equation and Notation Workbench (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
