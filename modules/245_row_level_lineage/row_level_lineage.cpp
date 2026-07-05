#include "row_level_lineage.hpp"

namespace row_level_lineage {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Row-level lineage (Databases). Awaits wire-up.";
    return s;
}

}
