#include "peripheral_register_designer.hpp"

namespace peripheral_register_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Peripheral register designer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
