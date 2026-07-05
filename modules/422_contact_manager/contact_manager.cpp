#include "contact_manager.hpp"

namespace contact_manager {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Contact manager (Office productivity). Awaits wire-up.";
    return s;
}

}
