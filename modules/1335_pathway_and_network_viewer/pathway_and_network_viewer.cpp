#include "pathway_and_network_viewer.hpp"

namespace pathway_and_network_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pathway and Network Viewer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
