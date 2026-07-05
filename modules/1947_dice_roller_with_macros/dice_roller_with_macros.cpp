#include "dice_roller_with_macros.hpp"

namespace dice_roller_with_macros {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dice Roller With Macros (Tabletop RPG and world building). Awaits wire-up.";
    return s;
}

}
