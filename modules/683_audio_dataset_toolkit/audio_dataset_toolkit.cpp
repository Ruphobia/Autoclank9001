#include "audio_dataset_toolkit.hpp"

namespace audio_dataset_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Audio dataset toolkit (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
