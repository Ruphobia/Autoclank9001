#include "rigging_studio.hpp"

namespace rigging_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rigging Studio (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
