#include "genome_annotator.hpp"

namespace genome_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genome annotator (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
