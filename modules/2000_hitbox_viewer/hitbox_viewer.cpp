#include "hitbox_viewer.hpp"

namespace hitbox_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hitbox Viewer (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
