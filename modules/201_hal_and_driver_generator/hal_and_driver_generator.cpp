#include "hal_and_driver_generator.hpp"

namespace hal_and_driver_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HAL and driver generator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
