#include "pathway_and_ontology_enricher.hpp"

namespace pathway_and_ontology_enricher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Pathway and ontology enricher (Microbiology, virology, and immunology). Awaits wire-up.";
    return s;
}

}
