#include "discrete_event_simulator.hpp"

namespace discrete_event_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Discrete-event simulator (Logistics, supply chain, operations research). Awaits wire-up.";
    return s;
}

}
