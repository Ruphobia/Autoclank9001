#include "trail_cam_image_manager.hpp"

namespace trail_cam_image_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Trail-Cam Image Manager (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
