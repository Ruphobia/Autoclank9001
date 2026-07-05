// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/1238_bicycle_gear_inch_calculator/bicycle_gear_inch_calculator.hpp"

namespace {

testing::TestOutcome run_bicycle_gear_inch_calculator() {
    bicycle_gear_inch_calculator::init();
    auto s = bicycle_gear_inch_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    bicycle_gear_inch_calculator::shutdown();
    return testing::ok();
}

const int _reg_bicycle_gear_inch_calculator = testing::register_test(
    "bicycle_gear_inch_calculator",
    "1238_bicycle_gear_inch_calculator: stub status check",
    &run_bicycle_gear_inch_calculator);

}
