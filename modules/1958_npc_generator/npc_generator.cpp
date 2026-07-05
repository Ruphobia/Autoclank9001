#include "npc_generator.hpp"

namespace npc_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NPC Generator (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
