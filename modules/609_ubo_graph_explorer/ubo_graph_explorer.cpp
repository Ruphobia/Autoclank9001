#include "ubo_graph_explorer.hpp"

namespace ubo_graph_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: UBO Graph Explorer (KYC, identity, compliance verification). Awaits wire-up.";
    return s;
}

}
