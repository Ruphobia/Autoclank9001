#include "genomic_variant_annotator.hpp"

namespace genomic_variant_annotator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Genomic Variant Annotator (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
