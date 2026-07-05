#include "firmware_unpacker.hpp"

namespace firmware_unpacker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Firmware unpacker (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
