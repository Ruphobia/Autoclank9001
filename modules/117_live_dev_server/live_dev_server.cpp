#include "live_dev_server.hpp"

namespace live_dev_server {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Live dev server (Web development). Awaits wire-up.";
    return s;
}

}
