#include "network_log_triage.hpp"

namespace network_log_triage {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Network log triage (Web development). Awaits wire-up.";
    return s;
}

}
