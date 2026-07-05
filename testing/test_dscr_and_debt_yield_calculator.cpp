// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/537_dscr_and_debt_yield_calculator/dscr_and_debt_yield_calculator.hpp"

namespace {

testing::TestOutcome run_dscr_and_debt_yield_calculator() {
    dscr_and_debt_yield_calculator::init();
    auto s = dscr_and_debt_yield_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dscr_and_debt_yield_calculator::shutdown();
    return testing::ok();
}

const int _reg_dscr_and_debt_yield_calculator = testing::register_test(
    "dscr_and_debt_yield_calculator",
    "537_dscr_and_debt_yield_calculator: stub status check",
    &run_dscr_and_debt_yield_calculator);

}
