#include "rig_control_bridge.hpp"

namespace rig_control_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rig control bridge (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
