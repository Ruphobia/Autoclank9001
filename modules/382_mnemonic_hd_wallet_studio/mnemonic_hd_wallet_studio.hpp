#pragma once

#include <string>

// Stub module for the "Mnemonic & HD Wallet Studio" tool entry.
// Cryptography and blockchain.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace mnemonic_hd_wallet_studio {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
