#include "spray_mix_calculator.hpp"

namespace spray_mix_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spray Mix Calculator (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
