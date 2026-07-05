#include "lca_inventory_builder.hpp"

namespace lca_inventory_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: LCA inventory builder (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
