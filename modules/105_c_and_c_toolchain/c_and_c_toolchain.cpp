#include "c_and_c_toolchain.hpp"

namespace c_and_c_toolchain {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: C and C++ toolchain (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
