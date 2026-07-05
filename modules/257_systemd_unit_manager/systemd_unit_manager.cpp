#include "systemd_unit_manager.hpp"

namespace systemd_unit_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Systemd unit manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
