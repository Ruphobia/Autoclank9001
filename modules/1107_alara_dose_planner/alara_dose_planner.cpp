#include "alara_dose_planner.hpp"

namespace alara_dose_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ALARA dose planner (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
