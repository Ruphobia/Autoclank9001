#include "osint_aggregator.hpp"

namespace osint_aggregator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: OSINT Aggregator (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
