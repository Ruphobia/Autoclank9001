#include "path_planner.hpp"

namespace path_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Path planner (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
