#include "primer_designer.hpp"

namespace primer_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Primer Designer (Biology and bioinformatics). Awaits wire-up.";
    return s;
}

}
