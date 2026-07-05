#include "reference_resolver.hpp"

namespace reference_resolver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Reference resolver (Documents and reading). Awaits wire-up.";
    return s;
}

}
