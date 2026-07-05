#include "conservation_prioritization_solver.hpp"

namespace conservation_prioritization_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Conservation prioritization solver (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
