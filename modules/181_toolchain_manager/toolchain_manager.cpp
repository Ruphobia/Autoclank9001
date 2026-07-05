#include "toolchain_manager.hpp"

namespace toolchain_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Toolchain manager (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
