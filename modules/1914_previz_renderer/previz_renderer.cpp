#include "previz_renderer.hpp"

namespace previz_renderer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Previz Renderer (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
