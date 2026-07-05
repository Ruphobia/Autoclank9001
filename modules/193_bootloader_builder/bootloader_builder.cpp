#include "bootloader_builder.hpp"

namespace bootloader_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bootloader builder (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
