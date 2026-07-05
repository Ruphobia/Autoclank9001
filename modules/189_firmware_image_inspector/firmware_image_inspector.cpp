#include "firmware_image_inspector.hpp"

namespace firmware_image_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Firmware image inspector (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
