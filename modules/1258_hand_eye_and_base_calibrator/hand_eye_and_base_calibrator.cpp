#include "hand_eye_and_base_calibrator.hpp"

namespace hand_eye_and_base_calibrator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hand-eye and base calibrator (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
