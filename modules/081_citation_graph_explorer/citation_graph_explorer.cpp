#include "citation_graph_explorer.hpp"

namespace citation_graph_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Citation Graph Explorer (Memory and knowledge accrual). Awaits wire-up.";
    return s;
}

}
