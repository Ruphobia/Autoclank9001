#include "crew_scheduler.hpp"

namespace crew_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crew scheduler (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
