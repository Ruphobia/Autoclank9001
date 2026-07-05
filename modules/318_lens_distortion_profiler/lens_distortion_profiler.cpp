#include "lens_distortion_profiler.hpp"

namespace lens_distortion_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lens Distortion Profiler (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
