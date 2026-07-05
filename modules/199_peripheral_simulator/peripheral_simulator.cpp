#include "peripheral_simulator.hpp"

namespace peripheral_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Peripheral simulator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
