#include "hamiltonian_builder.hpp"

namespace hamiltonian_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Hamiltonian Builder (Quantum computing). Awaits wire-up.";
    return s;
}

}
