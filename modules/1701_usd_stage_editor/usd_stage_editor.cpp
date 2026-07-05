#include "usd_stage_editor.hpp"

namespace usd_stage_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: USD Stage Editor (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
