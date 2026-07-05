#include "cipher_playground.hpp"

namespace cipher_playground {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Cipher Playground (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
