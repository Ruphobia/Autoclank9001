#include "tensor_network_simulator.hpp"

namespace tensor_network_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Tensor Network Simulator (Quantum computing). Awaits wire-up.";
    return s;
}

}
