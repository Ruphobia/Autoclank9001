#include "replication_monitor.hpp"

namespace replication_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Replication monitor (Databases). Awaits wire-up.";
    return s;
}

}
