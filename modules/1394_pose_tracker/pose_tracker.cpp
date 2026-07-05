#include "pose_tracker.hpp"

namespace pose_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pose tracker (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
