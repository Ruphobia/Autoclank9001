#include "statevector_simulator.hpp"

namespace statevector_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Statevector Simulator (Quantum computing). Awaits wire-up.";
    return s;
}

}
