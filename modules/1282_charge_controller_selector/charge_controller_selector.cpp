#include "charge_controller_selector.hpp"

namespace charge_controller_selector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Charge Controller Selector (Energy systems). Awaits wire-up.";
    return s;
}

}
