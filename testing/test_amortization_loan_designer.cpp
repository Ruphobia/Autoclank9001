// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/563_amortization_loan_designer/amortization_loan_designer.hpp"

namespace {

testing::TestOutcome run_amortization_loan_designer() {
    amortization_loan_designer::init();
    auto s = amortization_loan_designer::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    amortization_loan_designer::shutdown();
    return testing::ok();
}

const int _reg_amortization_loan_designer = testing::register_test(
    "amortization_loan_designer",
    "563_amortization_loan_designer: stub status check",
    &run_amortization_loan_designer);

}
