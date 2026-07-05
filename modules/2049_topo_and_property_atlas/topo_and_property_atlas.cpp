#include "topo_and_property_atlas.hpp"

namespace topo_and_property_atlas {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Topo and Property Atlas (Hunting, fishing, trapping, wildlife tracking). Awaits wire-up.";
    return s;
}

}
