#include "image_diff.hpp"

namespace image_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Image diff (Image and video). Awaits wire-up.";
    return s;
}

}
