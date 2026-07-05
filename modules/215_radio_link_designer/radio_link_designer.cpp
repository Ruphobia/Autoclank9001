#include "radio_link_designer.hpp"

namespace radio_link_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Radio link designer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
