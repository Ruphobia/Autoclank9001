// SPDX-License-Identifier: GPL-3.0-or-later
#include "fft_and_spectral_inspector.hpp"

namespace fft_and_spectral_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FFT and spectral inspector (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
