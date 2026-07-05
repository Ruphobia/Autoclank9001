#include "gantt_timeline.hpp"

namespace gantt_timeline {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gantt timeline (Office productivity). Awaits wire-up.";
    return s;
}

}
