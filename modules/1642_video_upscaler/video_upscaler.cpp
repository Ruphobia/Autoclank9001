#include "video_upscaler.hpp"

namespace video_upscaler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Video upscaler (Image and video). Awaits wire-up.";
    return s;
}

}
