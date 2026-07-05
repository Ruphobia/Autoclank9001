#pragma once

#include <string>

// Stub module for the "Crypto Cost-Basis Tracker" tool entry.
// Finance, markets, accounting.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace crypto_cost_basis_tracker {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
