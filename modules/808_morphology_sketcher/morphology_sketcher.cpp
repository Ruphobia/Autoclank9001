#include "morphology_sketcher.hpp"

namespace morphology_sketcher {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Morphology Sketcher (Linguistics and conlanging). Awaits wire-up.";
    return s;
}

}
