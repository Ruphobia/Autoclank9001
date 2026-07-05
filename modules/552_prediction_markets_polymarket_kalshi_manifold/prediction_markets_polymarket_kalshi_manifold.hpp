#pragma once

#include <string>

// Stub module for the "Prediction markets (Polymarket, Kalshi, Manifold)" tool entry.
// Finance, markets, accounting.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace prediction_markets_polymarket_kalshi_manifold {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
