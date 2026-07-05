#include "experiment_tracker.hpp"

namespace experiment_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Experiment tracker (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
