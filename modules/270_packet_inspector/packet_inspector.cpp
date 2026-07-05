#include "packet_inspector.hpp"

namespace packet_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Packet inspector (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
