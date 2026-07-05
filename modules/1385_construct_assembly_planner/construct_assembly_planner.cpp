#include "construct_assembly_planner.hpp"

namespace construct_assembly_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Construct assembly planner (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
