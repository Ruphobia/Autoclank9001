#include "circuit_builder.hpp"

namespace circuit_builder {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Circuit Builder (Quantum computing). Awaits wire-up.";
    return s;
}

}
