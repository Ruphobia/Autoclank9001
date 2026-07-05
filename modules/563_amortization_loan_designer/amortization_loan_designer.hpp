#pragma once

#include <string>

// Stub module for the "Amortization & Loan Designer" tool entry.
// Finance, markets, accounting.
// Real implementation deferred; this file commits the namespace API so
// downstream code can wire to it now and the smoke test can hold the
// surface stable.
namespace amortization_loan_designer {

struct Status {
    bool        ready = false;
    std::string detail;
};

void init();
void shutdown();
Status status();

}
