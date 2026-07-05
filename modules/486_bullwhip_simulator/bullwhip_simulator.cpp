#include "bullwhip_simulator.hpp"

namespace bullwhip_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Bullwhip simulator (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
