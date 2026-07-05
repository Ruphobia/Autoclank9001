#include "spectrometer_designer.hpp"

namespace spectrometer_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrometer Designer (Optics and photonics). Awaits wire-up.";
    return s;
}

}
