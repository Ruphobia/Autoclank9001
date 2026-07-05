#include "spectrogram_analyzer.hpp"

namespace spectrogram_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrogram analyzer (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
