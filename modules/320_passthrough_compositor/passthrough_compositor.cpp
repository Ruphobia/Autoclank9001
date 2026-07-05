#include "passthrough_compositor.hpp"

namespace passthrough_compositor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Passthrough Compositor (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
