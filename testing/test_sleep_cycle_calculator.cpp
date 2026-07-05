// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2157_sleep_cycle_calculator/sleep_cycle_calculator.hpp"

namespace {

testing::TestOutcome run_sleep_cycle_calculator() {
    sleep_cycle_calculator::init();
    auto s = sleep_cycle_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    sleep_cycle_calculator::shutdown();
    return testing::ok();
}

const int _reg_sleep_cycle_calculator = testing::register_test(
    "sleep_cycle_calculator",
    "2157_sleep_cycle_calculator: stub status check",
    &run_sleep_cycle_calculator);

}
