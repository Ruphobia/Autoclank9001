#include "keyframe_animator.hpp"

namespace keyframe_animator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Keyframe Animator (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
