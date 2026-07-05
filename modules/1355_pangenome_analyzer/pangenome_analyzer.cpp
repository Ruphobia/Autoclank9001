#include "pangenome_analyzer.hpp"

namespace pangenome_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pangenome analyzer (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
