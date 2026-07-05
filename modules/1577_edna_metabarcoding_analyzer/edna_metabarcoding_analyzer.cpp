#include "edna_metabarcoding_analyzer.hpp"

namespace edna_metabarcoding_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: eDNA metabarcoding analyzer (Ecology, conservation, sustainability). Awaits wire-up.";
    return s;
}

}
