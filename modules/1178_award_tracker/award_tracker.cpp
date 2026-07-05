#include "award_tracker.hpp"

namespace award_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Award tracker (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}
