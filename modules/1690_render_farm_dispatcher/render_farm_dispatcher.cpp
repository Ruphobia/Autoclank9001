#include "render_farm_dispatcher.hpp"

namespace render_farm_dispatcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Render Farm Dispatcher (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
