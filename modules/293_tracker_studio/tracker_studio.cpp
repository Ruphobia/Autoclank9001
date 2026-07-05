#include "tracker_studio.hpp"

namespace tracker_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tracker Studio (Game development). Awaits wire-up.";
    return s;
}

}
