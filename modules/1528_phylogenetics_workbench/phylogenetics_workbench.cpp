#include "phylogenetics_workbench.hpp"

namespace phylogenetics_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Phylogenetics workbench (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
