#include "character_sheet_forge.hpp"

namespace character_sheet_forge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Character Sheet Forge (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
