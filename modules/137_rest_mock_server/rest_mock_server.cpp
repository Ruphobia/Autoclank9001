#include "rest_mock_server.hpp"

namespace rest_mock_server {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: REST mock server (Web development). Awaits wire-up.";
    return s;
}

}
