#include "heat_transfer_solver.hpp"

namespace heat_transfer_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Heat-transfer solver (Physics and math). Awaits wire-up.";
    return s;
}

}
