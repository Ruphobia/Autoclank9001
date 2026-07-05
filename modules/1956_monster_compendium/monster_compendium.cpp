// SPDX-License-Identifier: GPL-3.0-or-later
#include "monster_compendium.hpp"

namespace monster_compendium {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Monster Compendium (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
