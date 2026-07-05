#include "dynamics_processor_suite.hpp"

namespace dynamics_processor_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Dynamics processor suite (Signal processing and audio). Awaits wire-up.";
    return s;
}

}
