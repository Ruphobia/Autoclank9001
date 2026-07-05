#include "bayesian_sandbox.hpp"

namespace bayesian_sandbox {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bayesian sandbox (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
