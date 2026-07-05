#include "unicode_inspector.hpp"

namespace unicode_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Unicode Inspector (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
