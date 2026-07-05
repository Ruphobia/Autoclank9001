#include "build_orchestrator.hpp"

namespace build_orchestrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Build orchestrator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
