#include "outline_editor.hpp"

namespace outline_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Outline editor (Office productivity). Awaits wire-up.";
    return s;
}

}
