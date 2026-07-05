#include "certificate_manager.hpp"

namespace certificate_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Certificate manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
