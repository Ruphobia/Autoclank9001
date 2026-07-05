#include "errata_tracker.hpp"

namespace errata_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Errata tracker (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
