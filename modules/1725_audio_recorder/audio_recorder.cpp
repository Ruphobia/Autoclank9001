#include "audio_recorder.hpp"

namespace audio_recorder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Audio recorder (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
