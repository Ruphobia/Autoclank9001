#include "vpn_and_tunnel_manager.hpp"

namespace vpn_and_tunnel_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: VPN and tunnel manager (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
