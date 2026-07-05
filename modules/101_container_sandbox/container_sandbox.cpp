#include "container_sandbox.hpp"

namespace container_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Container sandbox (Software development). Awaits wire-up.";
    return s;
}

}
