#include "go_toolchain.hpp"

namespace go_toolchain {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Go toolchain (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
