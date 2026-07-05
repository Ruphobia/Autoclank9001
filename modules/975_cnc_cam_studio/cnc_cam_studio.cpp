#include "cnc_cam_studio.hpp"

namespace cnc_cam_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CNC CAM Studio (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
