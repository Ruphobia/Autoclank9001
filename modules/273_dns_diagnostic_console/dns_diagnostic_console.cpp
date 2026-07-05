#include "dns_diagnostic_console.hpp"

namespace dns_diagnostic_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: DNS diagnostic console (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
