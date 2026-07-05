#include "power_profiler.hpp"

namespace power_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Power profiler (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
