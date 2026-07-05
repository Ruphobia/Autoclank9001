#include "ray_trace_optics_bench.hpp"

namespace ray_trace_optics_bench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Ray-trace optics bench (Physics and math). Awaits wire-up.";
    return s;
}

}
