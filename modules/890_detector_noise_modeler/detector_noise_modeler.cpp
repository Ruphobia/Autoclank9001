#include "detector_noise_modeler.hpp"

namespace detector_noise_modeler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Detector Noise Modeler (Optics and photonics). Awaits wire-up.";
    return s;
}

}
