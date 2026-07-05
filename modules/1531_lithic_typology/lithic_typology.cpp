#include "lithic_typology.hpp"

namespace lithic_typology {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lithic typology (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
