// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2119_ration_and_caloric_needs_calculator/ration_and_caloric_needs_calculator.hpp"

namespace {

testing::TestOutcome run_ration_and_caloric_needs_calculator() {
    ration_and_caloric_needs_calculator::init();
    auto s = ration_and_caloric_needs_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    ration_and_caloric_needs_calculator::shutdown();
    return testing::ok();
}

const int _reg_ration_and_caloric_needs_calculator = testing::register_test(
    "ration_and_caloric_needs_calculator",
    "2119_ration_and_caloric_needs_calculator: stub status check",
    &run_ration_and_caloric_needs_calculator);

}
