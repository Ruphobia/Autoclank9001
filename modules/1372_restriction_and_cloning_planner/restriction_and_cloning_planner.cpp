#include "restriction_and_cloning_planner.hpp"

namespace restriction_and_cloning_planner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Restriction and cloning planner (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
