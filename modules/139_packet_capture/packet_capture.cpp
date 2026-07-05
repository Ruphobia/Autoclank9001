#include "packet_capture.hpp"

namespace packet_capture {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Packet capture (Web development). Awaits wire-up.";
    return s;
}

}
