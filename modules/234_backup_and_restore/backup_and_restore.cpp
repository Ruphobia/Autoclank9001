#include "backup_and_restore.hpp"

namespace backup_and_restore {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Backup and restore (Databases). Awaits wire-up.";
    return s;
}

}
