#pragma once

#include <string>

// Stub module for the "XR Input Recorder" tool entry.
// VR / AR / XR.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace xr_input_recorder {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
