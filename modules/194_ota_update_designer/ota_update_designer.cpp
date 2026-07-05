#include "ota_update_designer.hpp"

namespace ota_update_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OTA update designer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
