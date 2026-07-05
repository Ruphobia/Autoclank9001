#include "texture_and_material_tools.hpp"

namespace texture_and_material_tools {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Texture and material tools (Image and video). Awaits wire-up.";
    return s;
}

}
