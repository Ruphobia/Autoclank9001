#include "inpainter.hpp"

namespace inpainter {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Inpainter (Image and video). Awaits wire-up.";
    return s;
}

}
