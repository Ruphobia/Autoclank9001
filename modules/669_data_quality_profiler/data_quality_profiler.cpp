#include "data_quality_profiler.hpp"

namespace data_quality_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Data quality profiler (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
