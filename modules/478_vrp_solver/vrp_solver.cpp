#include "vrp_solver.hpp"

namespace vrp_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: VRP solver (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
