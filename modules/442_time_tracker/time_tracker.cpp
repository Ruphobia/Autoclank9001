#include "time_tracker.hpp"

namespace time_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Time tracker (Project management). Awaits wire-up.";
    return s;
}

}
