#include "rtos_inspector.hpp"

namespace rtos_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: RTOS inspector (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
