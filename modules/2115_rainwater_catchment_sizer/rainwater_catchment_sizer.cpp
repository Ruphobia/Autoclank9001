#include "rainwater_catchment_sizer.hpp"

namespace rainwater_catchment_sizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rainwater catchment sizer (Survival, emergency preparedness, off-grid). Awaits wire-up.";
    return s;
}

}
