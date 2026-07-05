#pragma once

#include <string>

// Stub module for the "Camera calibrator" tool entry.
// Robotics and 3D.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace camera_calibrator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
