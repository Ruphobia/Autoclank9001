#include "mass_spectrum_workbench.hpp"

namespace mass_spectrum_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mass spectrum workbench (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
