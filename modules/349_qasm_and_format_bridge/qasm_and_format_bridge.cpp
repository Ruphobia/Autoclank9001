#include "qasm_and_format_bridge.hpp"

namespace qasm_and_format_bridge {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: QASM and Format Bridge (Quantum computing). Awaits wire-up.";
    return s;
}

}
