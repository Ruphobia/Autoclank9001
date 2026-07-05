#include "dosing_calculator.hpp"

namespace dosing_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dosing calculator (Hobbies: gardening, beekeeping, aquariums, pets). Awaits wire-up.";
    return s;
}

}
