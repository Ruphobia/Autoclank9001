#include "density_and_mass_calculator.hpp"

namespace density_and_mass_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Density And Mass Calculator (Materials science and metallurgy). Awaits wire-up.";
    return s;
}

}
