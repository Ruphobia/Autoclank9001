#include "stereonet.hpp"

namespace stereonet {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stereonet (Geology, GIS, and earth sciences). Awaits wire-up.";
    return s;
}

}
