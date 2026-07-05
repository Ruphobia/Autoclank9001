// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1141_link_budget_calculator/link_budget_calculator.hpp"

namespace {

testing::TestOutcome run_link_budget_calculator() {
    link_budget_calculator::init();
    auto s = link_budget_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    link_budget_calculator::shutdown();
    return testing::ok();
}

const int _reg_link_budget_calculator = testing::register_test(
    "link_budget_calculator",
    "1141_link_budget_calculator: stub status check",
    &run_link_budget_calculator);

}
