#include "phoneme_inventory_editor.hpp"

namespace phoneme_inventory_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phoneme Inventory Editor (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
