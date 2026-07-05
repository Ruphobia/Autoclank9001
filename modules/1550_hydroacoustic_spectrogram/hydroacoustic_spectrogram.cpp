#include "hydroacoustic_spectrogram.hpp"

namespace hydroacoustic_spectrogram {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hydroacoustic spectrogram (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
