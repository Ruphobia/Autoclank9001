#include "stocking_and_bioload_calculator.hpp"

namespace stocking_and_bioload_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stocking and bioload calculator (Hobbies: gardening, beekeeping, aquariums, pets). Awaits wire-up.";
    return s;
}

}
