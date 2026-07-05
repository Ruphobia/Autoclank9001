#include "http_intercepting_proxy.hpp"

namespace http_intercepting_proxy {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: HTTP intercepting proxy (Web development). Awaits wire-up.";
    return s;
}

}
