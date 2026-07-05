#include "quantization_bench.hpp"

namespace quantization_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Quantization bench (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
