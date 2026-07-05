#include "tomography_suite.hpp"

namespace tomography_suite {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tomography Suite (Quantum computing). Awaits wire-up.";
    return s;
}

}
