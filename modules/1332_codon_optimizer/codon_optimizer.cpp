#include "codon_optimizer.hpp"

namespace codon_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Codon Optimizer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
