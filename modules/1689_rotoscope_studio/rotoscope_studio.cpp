#include "rotoscope_studio.hpp"

namespace rotoscope_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rotoscope Studio (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
