#include "scope_change_monitor.hpp"

namespace scope_change_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Scope-change monitor (Project management). Awaits wire-up.";
    return s;
}

}
