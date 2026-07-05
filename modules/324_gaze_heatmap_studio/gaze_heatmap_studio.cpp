#include "gaze_heatmap_studio.hpp"

namespace gaze_heatmap_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gaze Heatmap Studio (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
