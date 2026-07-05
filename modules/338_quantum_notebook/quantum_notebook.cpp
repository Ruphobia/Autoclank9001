#include "quantum_notebook.hpp"

namespace quantum_notebook {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Quantum Notebook (Quantum computing). Awaits wire-up.";
    return s;
}

}
