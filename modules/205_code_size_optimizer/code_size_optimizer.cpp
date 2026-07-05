#include "code_size_optimizer.hpp"

namespace code_size_optimizer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Code-size optimizer (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
