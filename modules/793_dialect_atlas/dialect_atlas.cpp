#include "dialect_atlas.hpp"

namespace dialect_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dialect Atlas (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
