#include "flavor_aroma_pairing_explorer.hpp"

namespace flavor_aroma_pairing_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Flavor & Aroma Pairing Explorer (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
