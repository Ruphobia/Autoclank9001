#include "sprite_editor.hpp"

namespace sprite_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sprite Editor (Game development). Awaits wire-up.";
    return s;
}

}
