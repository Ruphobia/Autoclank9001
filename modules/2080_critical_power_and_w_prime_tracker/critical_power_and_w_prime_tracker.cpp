#include "critical_power_and_w_prime_tracker.hpp"

namespace critical_power_and_w_prime_tracker {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Critical power and W-prime tracker (Sports analytics, fitness, nutrition, coaching). Awaits wire-up.";
    return s;
}

}
