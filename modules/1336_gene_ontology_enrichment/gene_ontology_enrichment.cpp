#include "gene_ontology_enrichment.hpp"

namespace gene_ontology_enrichment {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Gene Ontology Enrichment (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
