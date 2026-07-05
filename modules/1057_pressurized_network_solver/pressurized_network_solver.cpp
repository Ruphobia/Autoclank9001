#include "pressurized_network_solver.hpp"

namespace pressurized_network_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pressurized network solver (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
