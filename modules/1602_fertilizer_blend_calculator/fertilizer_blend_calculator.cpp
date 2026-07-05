#include "fertilizer_blend_calculator.hpp"

namespace fertilizer_blend_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fertilizer Blend Calculator (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
