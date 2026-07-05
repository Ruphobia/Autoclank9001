#include "foraging_field_guide.hpp"

namespace foraging_field_guide {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Foraging field guide (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
