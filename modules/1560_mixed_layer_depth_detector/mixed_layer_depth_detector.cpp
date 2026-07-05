#include "mixed_layer_depth_detector.hpp"

namespace mixed_layer_depth_detector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mixed layer depth detector (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}
