#include "sleep_restriction_scheduler.hpp"

namespace sleep_restriction_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sleep restriction scheduler (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
