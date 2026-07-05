#include "usb_descriptor_builder.hpp"

namespace usb_descriptor_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: USB descriptor builder (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
