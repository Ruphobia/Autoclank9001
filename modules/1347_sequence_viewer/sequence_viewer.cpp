#include "sequence_viewer.hpp"

namespace sequence_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Sequence viewer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
