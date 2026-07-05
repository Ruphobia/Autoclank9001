#include "qec_workbench.hpp"

namespace qec_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: QEC Workbench (Quantum computing). Awaits wire-up.";
    return s;
}

}
