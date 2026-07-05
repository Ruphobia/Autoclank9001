#pragma once

#include <string>

// Stub module for the "Spatial Audio Mixer" tool entry.
// VR / AR / XR.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace spatial_audio_mixer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
