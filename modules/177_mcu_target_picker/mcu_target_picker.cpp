#include "mcu_target_picker.hpp"

namespace mcu_target_picker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MCU target picker (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
