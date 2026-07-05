#include "naca_generator.hpp"

namespace naca_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NACA Generator (Aerodynamics and CFD). Awaits wire-up.";
    return s;
}

}
