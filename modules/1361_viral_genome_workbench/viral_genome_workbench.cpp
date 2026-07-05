#include "viral_genome_workbench.hpp"

namespace viral_genome_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Viral genome workbench (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
