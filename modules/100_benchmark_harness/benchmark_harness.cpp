#include "benchmark_harness.hpp"

namespace benchmark_harness {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Benchmark harness (Software development). Awaits wire-up.";
    return s;
}

}
