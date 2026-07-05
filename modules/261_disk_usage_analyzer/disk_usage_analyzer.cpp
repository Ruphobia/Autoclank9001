#include "disk_usage_analyzer.hpp"

namespace disk_usage_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disk usage analyzer (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
