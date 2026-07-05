#include "tsp_playground.hpp"

namespace tsp_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TSP playground (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
