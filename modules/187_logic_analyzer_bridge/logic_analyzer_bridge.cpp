#include "logic_analyzer_bridge.hpp"

namespace logic_analyzer_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Logic analyzer bridge (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
