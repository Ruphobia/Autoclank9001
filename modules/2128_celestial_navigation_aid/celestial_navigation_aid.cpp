#include "celestial_navigation_aid.hpp"

namespace celestial_navigation_aid {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Celestial navigation aid (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
