#include "on_chip_debugger.hpp"

namespace on_chip_debugger {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: On-chip debugger (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
