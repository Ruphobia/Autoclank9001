#include "backup_scheduler.hpp"

namespace backup_scheduler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Backup scheduler (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
