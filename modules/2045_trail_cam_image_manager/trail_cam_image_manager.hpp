#pragma once

#include <string>

// Stub module for the "Trail-Cam Image Manager" tool entry.
// Hunting, fishing, trapping, wildlife tracking.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace trail_cam_image_manager {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
