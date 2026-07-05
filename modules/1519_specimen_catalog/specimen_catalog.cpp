#include "specimen_catalog.hpp"

namespace specimen_catalog {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Specimen catalog (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
