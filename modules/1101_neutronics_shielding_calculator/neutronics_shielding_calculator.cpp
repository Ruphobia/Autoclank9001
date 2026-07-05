#include "neutronics_shielding_calculator.hpp"

namespace neutronics_shielding_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Neutronics shielding calculator (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
