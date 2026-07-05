#include "input_overlay.hpp"

namespace input_overlay {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Input Overlay (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
