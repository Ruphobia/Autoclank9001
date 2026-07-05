#include "panorama_stitcher.hpp"

namespace panorama_stitcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Panorama stitcher (Image and video). Awaits wire-up.";
    return s;
}

}
