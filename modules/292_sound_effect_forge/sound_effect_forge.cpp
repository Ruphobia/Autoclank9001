#include "sound_effect_forge.hpp"

namespace sound_effect_forge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sound Effect Forge (Game development). Awaits wire-up.";
    return s;
}

}
