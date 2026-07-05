#include "hdr_and_tonemap.hpp"

namespace hdr_and_tonemap {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HDR and tonemap (Image and video). Awaits wire-up.";
    return s;
}

}
