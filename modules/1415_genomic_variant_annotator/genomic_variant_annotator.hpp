#pragma once

#include <string>

// Stub module for the "Genomic Variant Annotator" tool entry.
// Medical, pharmacology, healthcare.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace genomic_variant_annotator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
