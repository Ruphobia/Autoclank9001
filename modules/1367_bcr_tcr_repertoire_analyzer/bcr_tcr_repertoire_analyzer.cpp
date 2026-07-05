#include "bcr_tcr_repertoire_analyzer.hpp"

namespace bcr_tcr_repertoire_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BCR/TCR repertoire analyzer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
