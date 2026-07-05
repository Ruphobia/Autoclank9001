#include "save_format_editor.hpp"

namespace save_format_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Save Format Editor (Game development). Awaits wire-up.";
    return s;
}

}
