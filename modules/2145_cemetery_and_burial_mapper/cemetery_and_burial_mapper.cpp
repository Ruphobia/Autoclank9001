#include "cemetery_and_burial_mapper.hpp"

namespace cemetery_and_burial_mapper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cemetery and Burial Mapper (Detailed genealogy and family history). Awaits wire-up.";
    return s;
}

}
