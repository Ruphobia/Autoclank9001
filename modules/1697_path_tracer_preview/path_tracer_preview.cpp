#include "path_tracer_preview.hpp"

namespace path_tracer_preview {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Path Tracer Preview (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
