#include "pulse_schedule_editor.hpp"

namespace pulse_schedule_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pulse Schedule Editor (Quantum computing). Awaits wire-up.";
    return s;
}

}
