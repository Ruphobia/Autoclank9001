#include "mocap_cleanup.hpp"

namespace mocap_cleanup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mocap Cleanup (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
