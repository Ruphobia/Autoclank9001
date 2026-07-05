#include "state_machine_editor.hpp"

namespace state_machine_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: State Machine Editor (Game development). Awaits wire-up.";
    return s;
}

}
