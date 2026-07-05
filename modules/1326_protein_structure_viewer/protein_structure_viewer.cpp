#include "protein_structure_viewer.hpp"

namespace protein_structure_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Protein Structure Viewer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
