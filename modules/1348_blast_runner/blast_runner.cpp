#include "blast_runner.hpp"

namespace blast_runner {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: BLAST runner (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
