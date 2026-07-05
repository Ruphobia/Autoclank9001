#include "burnup_and_depletion_tool.hpp"

namespace burnup_and_depletion_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Burnup and depletion tool (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
