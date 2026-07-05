#include "secure_provisioning_console.hpp"

namespace secure_provisioning_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Secure provisioning console (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
