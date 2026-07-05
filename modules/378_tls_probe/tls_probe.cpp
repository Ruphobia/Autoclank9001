#include "tls_probe.hpp"

namespace tls_probe {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: TLS Probe (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
