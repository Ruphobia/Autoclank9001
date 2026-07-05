#include "lipsync_and_talking_head.hpp"

namespace lipsync_and_talking_head {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lipsync and talking head (Image and video). Awaits wire-up.";
    return s;
}

}
