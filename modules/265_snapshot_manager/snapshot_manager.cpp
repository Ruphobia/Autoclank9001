#include "snapshot_manager.hpp"

namespace snapshot_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Snapshot manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
