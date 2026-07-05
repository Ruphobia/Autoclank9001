#include "alignment_viewer.hpp"

namespace alignment_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Alignment viewer (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
