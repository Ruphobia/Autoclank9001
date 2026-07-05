#include "spectrogram_studio.hpp"

namespace spectrogram_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrogram Studio (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
