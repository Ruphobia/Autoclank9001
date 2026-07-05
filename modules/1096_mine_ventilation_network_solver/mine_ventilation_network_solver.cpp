#include "mine_ventilation_network_solver.hpp"

namespace mine_ventilation_network_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mine ventilation network solver (Heavy engineering: naval, petroleum, mining, nuclear). Awaits wire-up.";
    return s;
}

}
