#include "ecu_map_editor.hpp"

namespace ecu_map_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ECU Map Editor (Automotive and vehicle systems). Awaits wire-up.";
    return s;
}

}
