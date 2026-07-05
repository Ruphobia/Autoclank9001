#include "hand_pose_authoring.hpp"

namespace hand_pose_authoring {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hand Pose Authoring (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
