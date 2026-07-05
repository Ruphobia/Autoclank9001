#include "disinfection_ct_solver.hpp"

namespace disinfection_ct_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Disinfection CT solver (Civil utility (water, waste, sanitation)). Awaits wire-up.";
    return s;
}

}
