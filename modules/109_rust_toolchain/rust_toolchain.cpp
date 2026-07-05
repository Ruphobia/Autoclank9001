#include "rust_toolchain.hpp"

namespace rust_toolchain {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rust toolchain (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
