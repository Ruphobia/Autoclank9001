#include "thermodynamics_workbench.hpp"

namespace thermodynamics_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Thermodynamics workbench (Physics and math). Awaits wire-up.";
    return s;
}

}
