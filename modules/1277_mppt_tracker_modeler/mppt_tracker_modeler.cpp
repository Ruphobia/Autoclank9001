#include "mppt_tracker_modeler.hpp"

namespace mppt_tracker_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MPPT Tracker Modeler (Energy systems). Awaits wire-up.";
    return s;
}

}
