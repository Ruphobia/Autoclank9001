#include "schema_diff.hpp"

namespace schema_diff {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Schema diff (Databases). Awaits wire-up.";
    return s;
}

}
