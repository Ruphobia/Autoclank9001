#include "eval_harness.hpp"

namespace eval_harness {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Eval harness (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
