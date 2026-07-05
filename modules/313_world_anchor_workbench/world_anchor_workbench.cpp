#include "world_anchor_workbench.hpp"

namespace world_anchor_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: World Anchor Workbench (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
