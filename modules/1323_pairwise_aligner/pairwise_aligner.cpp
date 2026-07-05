#include "pairwise_aligner.hpp"

namespace pairwise_aligner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pairwise Aligner (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
