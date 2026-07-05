#include "image_generator.hpp"

namespace image_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Image generator (Image and video). Awaits wire-up.";
    return s;
}

}
