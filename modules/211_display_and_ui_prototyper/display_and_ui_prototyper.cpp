#include "display_and_ui_prototyper.hpp"

namespace display_and_ui_prototyper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Display and UI prototyper (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
