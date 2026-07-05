#include "photogrammetry.hpp"

namespace photogrammetry {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Photogrammetry (Image and video). Awaits wire-up.";
    return s;
}

}
