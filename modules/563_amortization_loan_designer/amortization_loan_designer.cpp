// SPDX-License-Identifier: GPL-3.0-or-later
#include "amortization_loan_designer.hpp"

namespace amortization_loan_designer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Amortization & Loan Designer (Finance, markets, accounting). Awaits wire-up.";
    return s;
}

}
