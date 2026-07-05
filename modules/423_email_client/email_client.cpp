#include "email_client.hpp"

namespace email_client {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Email client (Office productivity). Awaits wire-up.";
    return s;
}

}
