#include "tracking_pose_inspector.hpp"

namespace tracking_pose_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tracking Pose Inspector (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
