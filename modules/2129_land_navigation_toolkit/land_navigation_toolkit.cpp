#include "land_navigation_toolkit.hpp"

namespace land_navigation_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Land navigation toolkit (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
