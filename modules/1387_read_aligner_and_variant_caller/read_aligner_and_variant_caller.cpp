#include "read_aligner_and_variant_caller.hpp"

namespace read_aligner_and_variant_caller {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Read aligner and variant caller (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
