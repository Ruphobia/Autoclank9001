#include "compositor.hpp"

namespace compositor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Compositor (Image and video). Awaits wire-up.";
    return s;
}

}
