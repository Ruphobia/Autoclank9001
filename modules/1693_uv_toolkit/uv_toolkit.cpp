#include "uv_toolkit.hpp"

namespace uv_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: UV Toolkit (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
