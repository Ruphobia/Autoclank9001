#include "bus_traffic_replayer.hpp"

namespace bus_traffic_replayer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bus traffic replayer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
