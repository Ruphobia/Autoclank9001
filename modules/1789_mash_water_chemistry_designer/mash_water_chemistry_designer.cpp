#include "mash_water_chemistry_designer.hpp"

namespace mash_water_chemistry_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mash Water Chemistry Designer (Cooking, brewing, fermentation, food science). Awaits wire-up.";
    return s;
}

}
