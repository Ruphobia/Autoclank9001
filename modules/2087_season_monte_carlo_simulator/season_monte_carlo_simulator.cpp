#include "season_monte_carlo_simulator.hpp"

namespace season_monte_carlo_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Season Monte Carlo simulator (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
