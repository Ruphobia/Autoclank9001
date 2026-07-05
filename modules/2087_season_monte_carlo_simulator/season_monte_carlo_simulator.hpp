#pragma once

#include <string>

// Stub module for the "Season Monte Carlo simulator" tool entry.
// Sports analytics, fitness, nutrition, coaching.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace season_monte_carlo_simulator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
