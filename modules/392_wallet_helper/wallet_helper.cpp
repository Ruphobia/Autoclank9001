#include "wallet_helper.hpp"

namespace wallet_helper {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wallet Helper (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
