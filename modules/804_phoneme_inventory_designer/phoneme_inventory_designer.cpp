#include "phoneme_inventory_designer.hpp"

namespace phoneme_inventory_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phoneme Inventory Designer (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
