#include "device_tree_editor.hpp"

namespace device_tree_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Device tree editor (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
