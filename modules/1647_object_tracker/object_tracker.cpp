#include "object_tracker.hpp"

namespace object_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Object tracker (Image and video). Awaits wire-up.";
    return s;
}

}
