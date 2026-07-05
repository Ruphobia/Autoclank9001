#include "mnemonic_hd_wallet_studio.hpp"

namespace mnemonic_hd_wallet_studio {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mnemonic & HD Wallet Studio (Cryptography and blockchain). Awaits wire-up.";
    return s;
}

}
