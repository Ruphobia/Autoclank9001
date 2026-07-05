#include "asset_library.hpp"

namespace asset_library {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Asset Library (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
