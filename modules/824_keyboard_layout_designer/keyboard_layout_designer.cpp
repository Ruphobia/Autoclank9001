#include "keyboard_layout_designer.hpp"

namespace keyboard_layout_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Keyboard layout designer (Speech, language, and translation). Awaits wire-up.";
    return s;
}

}
