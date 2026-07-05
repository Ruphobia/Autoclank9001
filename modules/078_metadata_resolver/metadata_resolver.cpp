#include "metadata_resolver.hpp"

namespace metadata_resolver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Metadata Resolver (Memory and knowledge accrual). Awaits wire-up.";
    return s;
}

}
