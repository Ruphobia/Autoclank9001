#include "radiometry_photometry_bench.hpp"

namespace radiometry_photometry_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Radiometry Photometry Bench (Optics and photonics). Awaits wire-up.";
    return s;
}

}
