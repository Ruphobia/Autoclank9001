#include "lattice_and_tpms_infill_generator.hpp"

namespace lattice_and_tpms_infill_generator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Lattice and TPMS Infill Generator (Maker fab and personal manufacturing). Awaits wire-up.";
    return s;
}

}
