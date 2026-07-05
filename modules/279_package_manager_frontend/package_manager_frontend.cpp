#include "package_manager_frontend.hpp"

namespace package_manager_frontend {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Package manager frontend (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
