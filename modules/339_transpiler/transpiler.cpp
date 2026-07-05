#include "transpiler.hpp"

namespace transpiler {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Transpiler (Quantum computing). Awaits wire-up.";
    return s;
}

}
