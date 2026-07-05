#include "migration_tool.hpp"

namespace migration_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Migration tool (Databases). Awaits wire-up.";
    return s;
}

}
