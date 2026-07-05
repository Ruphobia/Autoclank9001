#pragma once

#include <string>

// Stub module for the "Octomap and occupancy editor" tool entry.
// Robotics and 3D.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace octomap_and_occupancy_editor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
