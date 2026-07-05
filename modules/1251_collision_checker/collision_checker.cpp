#include "collision_checker.hpp"

namespace collision_checker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Collision checker (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
