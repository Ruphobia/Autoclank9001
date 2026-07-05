#include "component_playground.hpp"

namespace component_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Component playground (Web development). Awaits wire-up.";
    return s;
}

}
