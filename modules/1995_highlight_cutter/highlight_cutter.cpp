#include "highlight_cutter.hpp"

namespace highlight_cutter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Highlight Cutter (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
