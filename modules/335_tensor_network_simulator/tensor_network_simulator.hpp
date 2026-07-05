#pragma once

#include <string>

// Stub module for the "Tensor Network Simulator" tool entry.
// Quantum computing.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace tensor_network_simulator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
