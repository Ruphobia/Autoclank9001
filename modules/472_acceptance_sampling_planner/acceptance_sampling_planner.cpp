#include "acceptance_sampling_planner.hpp"

namespace acceptance_sampling_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Acceptance sampling planner (Quality, safety, risk, and compliance). Awaits wire-up.";
    return s;
}

}
