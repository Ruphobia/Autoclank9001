#include "cfd_solver.hpp"

namespace cfd_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: CFD solver (Mechanical and manufacturing). Awaits wire-up.";
    return s;
}

}
