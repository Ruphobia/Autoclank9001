#include "color_grading.hpp"

namespace color_grading {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Color grading (Image and video). Awaits wire-up.";
    return s;
}

}
