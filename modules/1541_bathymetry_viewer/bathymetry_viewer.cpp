#include "bathymetry_viewer.hpp"

namespace bathymetry_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bathymetry viewer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
