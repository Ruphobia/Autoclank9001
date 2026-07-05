// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/982_tolerance_and_fit_calculator/tolerance_and_fit_calculator.hpp"

namespace {

testing::TestOutcome run_tolerance_and_fit_calculator() {
    tolerance_and_fit_calculator::init();
    auto s = tolerance_and_fit_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    tolerance_and_fit_calculator::shutdown();
    return testing::ok();
}

const int _reg_tolerance_and_fit_calculator = testing::register_test(
    "tolerance_and_fit_calculator",
    "982_tolerance_and_fit_calculator: stub status check",
    &run_tolerance_and_fit_calculator);

}
