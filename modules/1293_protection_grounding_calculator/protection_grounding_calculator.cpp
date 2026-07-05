#include "protection_grounding_calculator.hpp"

namespace protection_grounding_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Protection & Grounding Calculator (Energy systems). Awaits wire-up.";
    return s;
}

}
