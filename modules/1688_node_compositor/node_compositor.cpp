#include "node_compositor.hpp"

namespace node_compositor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Node Compositor (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
