#include "upscaler.hpp"

namespace upscaler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Upscaler (Image and video). Awaits wire-up.";
    return s;
}

}
