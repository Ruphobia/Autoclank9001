#include "compose_stack_runner.hpp"

namespace compose_stack_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Compose stack runner (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
