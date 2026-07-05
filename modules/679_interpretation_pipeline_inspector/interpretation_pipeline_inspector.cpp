#include "interpretation_pipeline_inspector.hpp"

namespace interpretation_pipeline_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Interpretation pipeline inspector (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
