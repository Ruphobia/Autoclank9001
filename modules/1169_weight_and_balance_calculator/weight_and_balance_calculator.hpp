#pragma once

#include <string>

// Stub module for the "Weight and balance calculator" tool entry.
// Aviation, amateur radio, and RF.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace weight_and_balance_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
