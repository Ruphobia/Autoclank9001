#include "fine_tuning_runner.hpp"

namespace fine_tuning_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Fine-tuning runner (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}
