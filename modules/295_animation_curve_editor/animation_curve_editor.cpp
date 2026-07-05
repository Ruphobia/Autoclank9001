#include "animation_curve_editor.hpp"

namespace animation_curve_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Animation Curve Editor (Game development). Awaits wire-up.";
    return s;
}

}
