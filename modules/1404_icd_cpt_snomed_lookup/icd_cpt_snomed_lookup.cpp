#include "icd_cpt_snomed_lookup.hpp"

namespace icd_cpt_snomed_lookup {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: ICD / CPT / SNOMED Lookup (Medical, pharmacology, healthcare). Awaits wire-up.";
    return s;
}

}
