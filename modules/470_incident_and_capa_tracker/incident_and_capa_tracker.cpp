#include "incident_and_capa_tracker.hpp"

namespace incident_and_capa_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Incident and CAPA tracker (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
