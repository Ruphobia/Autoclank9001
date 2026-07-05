#include "fits_viewer.hpp"

namespace fits_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FITS Viewer (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
