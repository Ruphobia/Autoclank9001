#include "pinmux_planner.hpp"

namespace pinmux_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pinmux planner (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
