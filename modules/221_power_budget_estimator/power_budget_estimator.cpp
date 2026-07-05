#include "power_budget_estimator.hpp"

namespace power_budget_estimator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Power budget estimator (Firmware and embedded). Awaits wire-up.";
    return s;
}

}
