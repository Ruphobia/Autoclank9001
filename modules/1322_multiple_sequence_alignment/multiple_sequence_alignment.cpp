#include "multiple_sequence_alignment.hpp"

namespace multiple_sequence_alignment {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Multiple Sequence Alignment (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
