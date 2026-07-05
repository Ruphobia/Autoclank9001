#include "cap_stack_and_waterfall_modeler.hpp"

namespace cap_stack_and_waterfall_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cap stack and waterfall modeler (Real estate, insurance, actuarial). Awaits wire-up.";
    return s;
}

}
