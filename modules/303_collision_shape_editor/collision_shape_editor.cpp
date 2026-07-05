#include "collision_shape_editor.hpp"

namespace collision_shape_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Collision Shape Editor (Game development). Awaits wire-up.";
    return s;
}

}
