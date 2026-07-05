#include "hardware_inventory.hpp"

namespace hardware_inventory {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hardware inventory (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
