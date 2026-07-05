#include "rehearsal_scheduler.hpp"

namespace rehearsal_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rehearsal Scheduler (Performing arts: theater, sound, lighting, stage). Awaits wire-up.";
    return s;
}

}
