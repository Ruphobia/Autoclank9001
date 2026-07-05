#include "can_bus_uds_inspector.hpp"

namespace can_bus_uds_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CAN Bus & UDS Inspector (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
