#include "monte_carlo_retirement_simulator.hpp"

namespace monte_carlo_retirement_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Monte Carlo Retirement Simulator (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
