#include "port_and_connection_viewer.hpp"

namespace port_and_connection_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Port and connection viewer (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
