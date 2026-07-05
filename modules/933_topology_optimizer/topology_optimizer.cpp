#include "topology_optimizer.hpp"

namespace topology_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Topology optimizer (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
