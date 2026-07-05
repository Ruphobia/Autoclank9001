#include "amr_profiler.hpp"

namespace amr_profiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: AMR profiler (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
