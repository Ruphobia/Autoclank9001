#include "container_manager.hpp"

namespace container_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Container manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
