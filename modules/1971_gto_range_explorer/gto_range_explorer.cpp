#include "gto_range_explorer.hpp"

namespace gto_range_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: GTO range explorer (Game playing assistants). Awaits wire-up.";
    return s;
}

}
