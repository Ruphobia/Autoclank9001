#include "state_inspector.hpp"

namespace state_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: State Inspector (Quantum computing). Awaits wire-up.";
    return s;
}

}
