#include "http_client.hpp"

namespace http_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HTTP client (Web development). Awaits wire-up.";
    return s;
}

}
