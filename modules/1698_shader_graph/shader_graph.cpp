#include "shader_graph.hpp"

namespace shader_graph {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Shader Graph (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
