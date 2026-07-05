#include "x_360_video_studio.hpp"

namespace x_360_video_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: 360 Video Studio (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
