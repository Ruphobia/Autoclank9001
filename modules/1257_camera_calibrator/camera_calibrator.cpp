#include "camera_calibrator.hpp"

namespace camera_calibrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Camera calibrator (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
