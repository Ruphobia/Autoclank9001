#include "wood_movement_calculator.hpp"

namespace wood_movement_calculator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wood Movement Calculator (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
