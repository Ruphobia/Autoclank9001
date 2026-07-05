#include "frustum_and_fov_visualizer.hpp"

namespace frustum_and_fov_visualizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Frustum and FOV Visualizer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
