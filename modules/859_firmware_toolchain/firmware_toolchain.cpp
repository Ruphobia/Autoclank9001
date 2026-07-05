#include "firmware_toolchain.hpp"

namespace firmware_toolchain {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Firmware toolchain (Electronics and EDA). Awaits wire-up.";
    return s;
}

}
