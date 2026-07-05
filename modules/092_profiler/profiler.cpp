#include "profiler.hpp"

namespace profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Profiler (Software development). Awaits wire-up.";
    return s;
}

}
