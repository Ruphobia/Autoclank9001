#include "world_bible.hpp"

namespace world_bible {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: World Bible (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
