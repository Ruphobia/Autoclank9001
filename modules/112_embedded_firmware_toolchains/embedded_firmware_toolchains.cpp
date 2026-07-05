#include "embedded_firmware_toolchains.hpp"

namespace embedded_firmware_toolchains {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Embedded firmware toolchains (Per-language toolchains). Awaits wire-up.";
    return s;
}

}
