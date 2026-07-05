#include "genome_assembler.hpp"

namespace genome_assembler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genome assembler (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
