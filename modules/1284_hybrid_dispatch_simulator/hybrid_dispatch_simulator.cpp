#include "hybrid_dispatch_simulator.hpp"

namespace hybrid_dispatch_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hybrid Dispatch Simulator (Energy systems). Awaits wire-up.";
    return s;
}

}
