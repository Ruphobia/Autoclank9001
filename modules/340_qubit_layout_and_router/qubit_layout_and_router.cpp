#include "qubit_layout_and_router.hpp"

namespace qubit_layout_and_router {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Qubit Layout and Router (Quantum computing). Awaits wire-up.";
    return s;
}

}
