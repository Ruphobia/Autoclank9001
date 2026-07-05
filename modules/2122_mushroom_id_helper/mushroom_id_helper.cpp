#include "mushroom_id_helper.hpp"

namespace mushroom_id_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mushroom ID helper (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
