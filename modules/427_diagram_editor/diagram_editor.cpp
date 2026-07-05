#include "diagram_editor.hpp"

namespace diagram_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Diagram editor (Office productivity). Awaits wire-up.";
    return s;
}

}
