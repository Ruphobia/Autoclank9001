#include "open_access_resolver.hpp"

namespace open_access_resolver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Open Access Resolver (Memory and knowledge accrual). Awaits wire-up.";
    return s;
}

}
