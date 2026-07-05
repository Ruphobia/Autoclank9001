#include "style_transfer.hpp"

namespace style_transfer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Style transfer (Image and video). Awaits wire-up.";
    return s;
}

}
