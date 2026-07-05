#include "shader_playground.hpp"

namespace shader_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Shader Playground (Game development). Awaits wire-up.";
    return s;
}

}
