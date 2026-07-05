// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/959_unit_and_conversion_calculator/unit_and_conversion_calculator.hpp"

namespace {

testing::TestOutcome run_unit_and_conversion_calculator() {
    unit_and_conversion_calculator::init();
    auto s = unit_and_conversion_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    unit_and_conversion_calculator::shutdown();
    return testing::ok();
}

const int _reg_unit_and_conversion_calculator = testing::register_test(
    "unit_and_conversion_calculator",
    "959_unit_and_conversion_calculator: stub status check",
    &run_unit_and_conversion_calculator);

}
