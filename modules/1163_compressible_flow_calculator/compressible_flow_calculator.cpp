#include "compressible_flow_calculator.hpp"

namespace compressible_flow_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Compressible Flow Calculator (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
