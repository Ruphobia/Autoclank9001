#include "em_field_solver.hpp"

namespace em_field_solver {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: EM field solver (Physics and math). Awaits wire-up.";
    return s;
}

}
