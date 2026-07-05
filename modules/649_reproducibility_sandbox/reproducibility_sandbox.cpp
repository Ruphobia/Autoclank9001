#include "reproducibility_sandbox.hpp"

namespace reproducibility_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reproducibility Sandbox (Academic and scholarly research). Awaits wire-up.";
    return s;
}

}
