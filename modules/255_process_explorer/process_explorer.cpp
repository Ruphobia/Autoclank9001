#include "process_explorer.hpp"

namespace process_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Process explorer (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
