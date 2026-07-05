#include "crypto_playground.hpp"

namespace crypto_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Crypto Playground (Cybersecurity and digital forensics). Awaits wire-up.";
    return s;
}

}
