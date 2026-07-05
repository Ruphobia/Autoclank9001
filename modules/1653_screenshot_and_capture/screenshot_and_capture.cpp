#include "screenshot_and_capture.hpp"

namespace screenshot_and_capture {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Screenshot and capture (Image and video). Awaits wire-up.";
    return s;
}

}
