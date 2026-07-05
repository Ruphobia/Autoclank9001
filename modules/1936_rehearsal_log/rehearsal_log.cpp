#include "rehearsal_log.hpp"

namespace rehearsal_log {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Rehearsal Log (Magic, juggling, stage illusion, performance). Awaits wire-up.";
    return s;
}

}
