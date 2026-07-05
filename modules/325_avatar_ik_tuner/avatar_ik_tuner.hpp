#pragma once

#include <string>

// Stub module for the "Avatar IK Tuner" tool entry.
// VR / AR / XR.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace avatar_ik_tuner {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
