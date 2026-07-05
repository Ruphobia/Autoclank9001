#include "stereo_view_inspector.hpp"

namespace stereo_view_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stereo View Inspector (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
