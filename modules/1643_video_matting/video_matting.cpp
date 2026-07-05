#include "video_matting.hpp"

namespace video_matting {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Video matting (Image and video). Awaits wire-up.";
    return s;
}

}
