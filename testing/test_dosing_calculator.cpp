// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2011_dosing_calculator/dosing_calculator.hpp"

namespace {

testing::TestOutcome run_dosing_calculator() {
    dosing_calculator::init();
    auto s = dosing_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    dosing_calculator::shutdown();
    return testing::ok();
}

const int _reg_dosing_calculator = testing::register_test(
    "dosing_calculator",
    "2011_dosing_calculator: stub status check",
    &run_dosing_calculator);

}
