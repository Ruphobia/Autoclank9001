#include "multi_echelon_inventory_simulator.hpp"

namespace multi_echelon_inventory_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multi-echelon inventory simulator (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
