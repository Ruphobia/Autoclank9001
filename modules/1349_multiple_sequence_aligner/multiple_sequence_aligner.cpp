#include "multiple_sequence_aligner.hpp"

namespace multiple_sequence_aligner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multiple sequence aligner (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
