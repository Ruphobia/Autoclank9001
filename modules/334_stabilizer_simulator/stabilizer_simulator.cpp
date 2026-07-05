#include "stabilizer_simulator.hpp"

namespace stabilizer_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stabilizer Simulator (Quantum computing). Awaits wire-up.";
    return s;
}

}
