#include "scrim_scheduler.hpp"

namespace scrim_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scrim Scheduler (Esports, speedrunning, competitive game analytics). Awaits wire-up.";
    return s;
}

}
