#include "image_stacker.hpp"

namespace image_stacker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Image Stacker (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
