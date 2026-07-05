#include "causal_inference_toolkit.hpp"

namespace causal_inference_toolkit {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Causal inference toolkit (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
