#include "network_interface_configurator.hpp"

namespace network_interface_configurator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Network interface configurator (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
