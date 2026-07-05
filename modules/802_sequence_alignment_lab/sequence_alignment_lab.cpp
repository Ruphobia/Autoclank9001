#include "sequence_alignment_lab.hpp"

namespace sequence_alignment_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sequence Alignment Lab (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
