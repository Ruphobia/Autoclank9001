#include "level_layout_studio.hpp"

namespace level_layout_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Level Layout Studio (Game development). Awaits wire-up.";
    return s;
}

}
