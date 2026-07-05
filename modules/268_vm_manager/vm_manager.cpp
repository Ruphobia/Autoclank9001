#include "vm_manager.hpp"

namespace vm_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: VM manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
