#include "irrigation_scheduler.hpp"

namespace irrigation_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Irrigation Scheduler (Agriculture and horticulture). Awaits wire-up.";
    return s;
}

}
