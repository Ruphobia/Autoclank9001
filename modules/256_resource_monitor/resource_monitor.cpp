#include "resource_monitor.hpp"

namespace resource_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Resource monitor (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
