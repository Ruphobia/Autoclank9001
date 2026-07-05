#include "encounter_builder.hpp"

namespace encounter_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Encounter Builder (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
