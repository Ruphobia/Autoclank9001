#include "signature_workbench.hpp"

namespace signature_workbench {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Signature Workbench (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
