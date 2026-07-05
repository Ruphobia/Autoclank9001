#include "performance_profiler.hpp"

namespace performance_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Performance profiler (General computing and sysadmin). Awaits wire-up.";
    return s;
}

}
