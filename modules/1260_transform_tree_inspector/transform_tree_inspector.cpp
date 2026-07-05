#include "transform_tree_inspector.hpp"

namespace transform_tree_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transform tree inspector (Robotics and 3D). Awaits wire-up.";
    return s;
}

}
