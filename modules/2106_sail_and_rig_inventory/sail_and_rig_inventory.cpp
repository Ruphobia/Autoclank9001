#include "sail_and_rig_inventory.hpp"

namespace sail_and_rig_inventory {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sail and rig inventory (Sailing, boating, navigation). Awaits wire-up.";
    return s;
}

}
