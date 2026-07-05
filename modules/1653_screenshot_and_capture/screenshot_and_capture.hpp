#pragma once

#include <string>

// Stub module for the "Screenshot and capture" tool entry.
// Image and video.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace screenshot_and_capture {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
