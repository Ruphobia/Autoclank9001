#include "paleogeographic_mapper.hpp"

namespace paleogeographic_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Paleogeographic mapper (Paleontology and archaeology). Awaits wire-up.";
    return s;
}

}
