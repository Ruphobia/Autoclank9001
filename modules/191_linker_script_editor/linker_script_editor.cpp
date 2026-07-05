#include "linker_script_editor.hpp"

namespace linker_script_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Linker script editor (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
