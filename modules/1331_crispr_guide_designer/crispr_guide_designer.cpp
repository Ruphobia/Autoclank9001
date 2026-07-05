#include "crispr_guide_designer.hpp"

namespace crispr_guide_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CRISPR Guide Designer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
