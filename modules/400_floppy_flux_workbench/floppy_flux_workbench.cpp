#include "floppy_flux_workbench.hpp"

namespace floppy_flux_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Floppy Flux Workbench (Vintage hardware preservation and retrocomputing). Awaits wire-up.";
    return s;
}

}
