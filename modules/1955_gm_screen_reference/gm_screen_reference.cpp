#include "gm_screen_reference.hpp"

namespace gm_screen_reference {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GM Screen Reference (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
