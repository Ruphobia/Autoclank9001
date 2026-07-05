#include "motion_capture_studio.hpp"

namespace motion_capture_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Motion capture studio (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
