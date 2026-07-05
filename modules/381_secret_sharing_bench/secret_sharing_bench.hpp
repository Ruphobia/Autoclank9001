#pragma once

#include <string>

// Stub module for the "Secret Sharing Bench" tool entry.
// Cryptography and blockchain.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace secret_sharing_bench {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
