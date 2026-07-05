#include "pose_mannequin.hpp"

namespace pose_mannequin {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pose mannequin (Visual arts: calligraphy, painting, sculpture). Awaits wire-up.";
    return s;
}

}
