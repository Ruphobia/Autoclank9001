#include "aperture_photometry.hpp"

namespace aperture_photometry {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Aperture Photometry (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
