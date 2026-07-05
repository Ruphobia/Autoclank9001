#include "y_dna_and_mtdna_haplogroup_explorer.hpp"

namespace y_dna_and_mtdna_haplogroup_explorer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Y-DNA and mtDNA Haplogroup Explorer (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
