#include "gaussian_beam_propagator.hpp"

namespace gaussian_beam_propagator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gaussian Beam Propagator (Optics and photonics). Awaits wire-up.";
    return s;
}

}
