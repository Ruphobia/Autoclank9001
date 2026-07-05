#include "ssh_and_remote_host_manager.hpp"

namespace ssh_and_remote_host_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: SSH and remote host manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
