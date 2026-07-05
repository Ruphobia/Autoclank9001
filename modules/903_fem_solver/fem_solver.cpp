#include "fem_solver.hpp"

namespace fem_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: FEM solver (Physics and math). Awaits wire-up.";
    return s;
}

}
