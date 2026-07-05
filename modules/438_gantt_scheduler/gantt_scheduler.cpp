#include "gantt_scheduler.hpp"

namespace gantt_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gantt scheduler (Project management). Awaits wire-up.";
    return s;
}

}
