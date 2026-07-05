#include "spatial_audio_mixer.hpp"

namespace spatial_audio_mixer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spatial Audio Mixer (VR / AR / XR). Awaits wire-up.";
    return s;
}

}
