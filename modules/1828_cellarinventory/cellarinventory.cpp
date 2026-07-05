#include "cellarinventory.hpp"

namespace cellarinventory {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CellarInventory (Coffee, tea, wine, spirits, beverages). Awaits wire-up.";
    return s;
}

}
