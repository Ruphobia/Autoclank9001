#include "zernike_wavefront_analyzer.hpp"

namespace zernike_wavefront_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Zernike Wavefront Analyzer (Optics and photonics). Awaits wire-up.";
    return s;
}

}
