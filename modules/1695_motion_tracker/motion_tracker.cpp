#include "motion_tracker.hpp"

namespace motion_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Motion Tracker (Animation, VFX, and 3D rendering). Awaits wire-up.";
    return s;
}

}
