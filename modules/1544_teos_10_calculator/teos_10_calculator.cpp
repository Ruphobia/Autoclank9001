#include "teos_10_calculator.hpp"

namespace teos_10_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TEOS-10 calculator (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
