#include "reality_check_scheduler.hpp"

namespace reality_check_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reality Check Scheduler (Sleep science, wellness, mental health). Awaits wire-up.";
    return s;
}

}
