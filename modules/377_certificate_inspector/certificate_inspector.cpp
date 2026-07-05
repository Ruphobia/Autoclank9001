#include "certificate_inspector.hpp"

namespace certificate_inspector {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Certificate Inspector (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
