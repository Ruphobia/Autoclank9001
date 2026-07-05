// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2194_cg_and_balance_calculator/cg_and_balance_calculator.hpp"

namespace {

testing::TestOutcome run_cg_and_balance_calculator() {
    cg_and_balance_calculator::init();
    auto s = cg_and_balance_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    cg_and_balance_calculator::shutdown();
    return testing::ok();
}

const int _reg_cg_and_balance_calculator = testing::register_test(
    "cg_and_balance_calculator",
    "2194_cg_and_balance_calculator: stub status check",
    &run_cg_and_balance_calculator);

}
