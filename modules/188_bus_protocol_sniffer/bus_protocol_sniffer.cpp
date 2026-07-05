#include "bus_protocol_sniffer.hpp"

namespace bus_protocol_sniffer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bus protocol sniffer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
