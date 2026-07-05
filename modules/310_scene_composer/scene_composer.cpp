#include "scene_composer.hpp"

namespace scene_composer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scene Composer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
