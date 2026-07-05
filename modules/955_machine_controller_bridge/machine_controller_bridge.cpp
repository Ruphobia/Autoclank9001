#include "machine_controller_bridge.hpp"

namespace machine_controller_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Machine controller bridge (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
