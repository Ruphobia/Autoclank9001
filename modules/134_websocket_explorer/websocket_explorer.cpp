#include "websocket_explorer.hpp"

namespace websocket_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: WebSocket explorer (Web development). Awaits wire-up.";
    return s;
}

}
