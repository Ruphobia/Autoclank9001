#include "spectrogram_annotator.hpp"

namespace spectrogram_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrogram annotator (Zoology, veterinary, wildlife). Awaits wire-up.";
    return s;
}

}
