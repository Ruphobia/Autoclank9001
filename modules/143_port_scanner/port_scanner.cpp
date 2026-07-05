#include "port_scanner.hpp"

namespace port_scanner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Port scanner (Web development). Awaits wire-up.";
    return s;
}

}
