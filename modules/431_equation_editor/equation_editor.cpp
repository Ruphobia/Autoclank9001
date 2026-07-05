#include "equation_editor.hpp"

namespace equation_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Equation editor (Office productivity). Awaits wire-up.";
    return s;
}

}
