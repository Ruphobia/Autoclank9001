#include "pose_and_depth_extractor.hpp"

namespace pose_and_depth_extractor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pose and depth extractor (Image and video). Awaits wire-up.";
    return s;
}

}
