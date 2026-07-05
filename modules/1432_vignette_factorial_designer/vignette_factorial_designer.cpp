#include "vignette_factorial_designer.hpp"

namespace vignette_factorial_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vignette factorial designer (Psychology and behavioral science). Awaits wire-up.";
    return s;
}

}
