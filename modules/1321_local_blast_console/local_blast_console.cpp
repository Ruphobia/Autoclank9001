#include "local_blast_console.hpp"

namespace local_blast_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Local BLAST Console (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
