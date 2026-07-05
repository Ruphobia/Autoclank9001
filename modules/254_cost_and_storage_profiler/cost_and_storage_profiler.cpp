#include "cost_and_storage_profiler.hpp"

namespace cost_and_storage_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cost and storage profiler (Databases). Awaits wire-up.";
    return s;
}

}
