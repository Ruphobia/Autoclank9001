#pragma once

#include <string>

// Stub module for the "DSCR and debt yield calculator" tool entry.
// Real estate, insurance, actuarial.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace dscr_and_debt_yield_calculator {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
