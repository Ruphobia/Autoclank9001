#pragma once

#include <string>

// Stub module for the "Protein Structure Viewer" tool entry.
// Biology and bioinformatics.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace protein_structure_viewer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
