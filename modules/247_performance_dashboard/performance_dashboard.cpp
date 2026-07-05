#include "performance_dashboard.hpp"

namespace performance_dashboard {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Performance dashboard (Databases). Awaits wire-up.";
    return s;
}

}
