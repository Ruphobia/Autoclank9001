#pragma once

#include <string>

// Stub module for the "Y-DNA and mtDNA Haplogroup Explorer" tool entry.
// Detailed genealogy and family history.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace y_dna_and_mtdna_haplogroup_explorer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
