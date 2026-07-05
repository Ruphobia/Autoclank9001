// SPDX-License-Identifier: GPL-3.0-or-later
#include "test_runner.hpp"
#include "../modules/2009_water_chemistry_calculator/water_chemistry_calculator.hpp"

namespace {

testing::TestOutcome run_water_chemistry_calculator() {
    water_chemistry_calculator::init();
    auto s = water_chemistry_calculator::status();
    if (s.ready)            return testing::fail("stub claims ready");
    if (s.detail.empty())   return testing::fail("stub detail empty");
    if (s.detail.find("stub") == std::string::npos)
        return testing::fail("stub detail not labeled stub");
    water_chemistry_calculator::shutdown();
    return testing::ok();
}

const int _reg_water_chemistry_calculator = testing::register_test(
    "water_chemistry_calculator",
    "2009_water_chemistry_calculator: stub status check",
    &run_water_chemistry_calculator);

}
