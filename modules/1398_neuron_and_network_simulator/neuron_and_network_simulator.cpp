#include "neuron_and_network_simulator.hpp"

namespace neuron_and_network_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Neuron and network simulator (Genomics, synthetic biology, and neuroscience). Awaits wire-up.";
    return s;
}

}
