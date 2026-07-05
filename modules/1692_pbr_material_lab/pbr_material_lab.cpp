#include "pbr_material_lab.hpp"

namespace pbr_material_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: PBR Material Lab (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
