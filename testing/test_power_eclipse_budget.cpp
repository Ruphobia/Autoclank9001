// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1142_power_eclipse_budget/power_eclipse_budget.hpp"

namespace {

testing::TestOutcome run_power_eclipse_budget() {
    power_eclipse_budget::init();
    auto s = power_eclipse_budget::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    power_eclipse_budget::shutdown();
    return testing::ok();
}

const int _reg_power_eclipse_budget = testing::register_test(
    "power_eclipse_budget",
    "1142_power_eclipse_budget: stub status check",
    &run_power_eclipse_budget);

}
