#include "octomap_and_occupancy_editor.hpp"

namespace octomap_and_occupancy_editor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Octomap and occupancy editor (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
