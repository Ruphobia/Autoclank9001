#include "quantum_chemistry_interface.hpp"

namespace quantum_chemistry_interface {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Quantum chemistry interface (Chemistry and lab). Awaits wire-up.";
    return s;
}

}
