#include "webxr_live_preview.hpp"

namespace webxr_live_preview {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: WebXR Live Preview (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
