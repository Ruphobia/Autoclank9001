#include "dye_recipe_tracker.hpp"

namespace dye_recipe_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dye Recipe Tracker (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}
