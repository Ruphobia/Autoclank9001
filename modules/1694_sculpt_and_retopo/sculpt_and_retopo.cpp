#include "sculpt_and_retopo.hpp"

namespace sculpt_and_retopo {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sculpt and Retopo (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
