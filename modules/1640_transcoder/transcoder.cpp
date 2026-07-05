#include "transcoder.hpp"

namespace transcoder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transcoder (Image and video). Awaits wire-up.";
    return s;
}

}
