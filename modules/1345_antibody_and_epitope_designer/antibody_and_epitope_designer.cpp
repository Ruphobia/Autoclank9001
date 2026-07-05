#include "antibody_and_epitope_designer.hpp"

namespace antibody_and_epitope_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Antibody and Epitope Designer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
