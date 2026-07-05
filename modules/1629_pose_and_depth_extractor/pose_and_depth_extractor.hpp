#pragma once

#include <string>

// Stub module for the "Pose and depth extractor" tool entry.
// Image and video.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace pose_and_depth_extractor {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
