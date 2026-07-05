#include "genealogy_graph.hpp"

namespace genealogy_graph {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genealogy Graph (Mythology, folklore, and comparative narrative). Awaits wire-up.";
    return s;
}

}
