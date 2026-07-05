#include "procedural_generation_sandbox.hpp"

namespace procedural_generation_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Procedural Generation Sandbox (Game development). Awaits wire-up.";
    return s;
}

}
