#include "hyperparameter_sweeper.hpp"

namespace hyperparameter_sweeper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hyperparameter sweeper (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
