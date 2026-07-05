#include "avatar_ik_tuner.hpp"

namespace avatar_ik_tuner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Avatar IK Tuner (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
