#pragma once

#include <string>

// Stub module for the "Trajectory designer" tool entry.
// Robotics and 3D.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace trajectory_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
