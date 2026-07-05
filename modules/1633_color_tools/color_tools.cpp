#include "color_tools.hpp"

namespace color_tools {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Color tools (Image and video). Awaits wire-up.";
    return s;
}

}
