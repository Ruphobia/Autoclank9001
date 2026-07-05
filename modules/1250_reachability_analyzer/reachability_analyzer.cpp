#include "reachability_analyzer.hpp"

namespace reachability_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reachability analyzer (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
