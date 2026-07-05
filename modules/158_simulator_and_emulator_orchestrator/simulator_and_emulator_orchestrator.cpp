#include "simulator_and_emulator_orchestrator.hpp"

namespace simulator_and_emulator_orchestrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Simulator and emulator orchestrator (Mobile development). Awaits wire-up.";
    return s;
}

}
