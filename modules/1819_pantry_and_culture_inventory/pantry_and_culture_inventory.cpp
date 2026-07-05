#include "pantry_and_culture_inventory.hpp"

namespace pantry_and_culture_inventory {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pantry and Culture Inventory (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
